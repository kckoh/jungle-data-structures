#####################################################################
# CS:APP Malloc Lab
# Handout files for students
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
######################################################################

***********
Main Files:
***********

mm.{c,h}	
	Your solution malloc package. mm.c is the file that you
	will be handing in, and is the only file you should modify.

mdriver.c	
	The malloc driver that tests your mm.c file

short{1,2}-bal.rep
	Two tiny tracefiles to help you get started. 

Makefile	
	Builds the driver

**********************************
Other support files for the driver
**********************************

config.h	Configures the malloc lab driver
fsecs.{c,h}	Wrapper function for the different timer packages
clock.{c,h}	Routines for accessing the Pentium and Alpha cycle counters
fcyc.{c,h}	Timer functions based on cycle counters
ftimer.{c,h}	Timer functions based on interval timers and gettimeofday()
memlib.{c,h}	Models the heap and sbrk function

*******************************
Building and running the driver
*******************************
To build the driver, type "make" to the shell.

To run the driver on a tiny test trace:

	unix> mdriver -V -f short1-bal.rep

The -V option prints out helpful tracing and summary information.

To get a list of the driver flags:

	unix> mdriver -h

---

# 🎯 Explicit Free List - 구현 가이드

## 1️⃣ **전체 구조: 두 개의 "세계"**

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🌍 HEAP 메모리 (물리적 세계)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

heap_listp
    ↓
[Prologue] → [Alloc A] → [Free B] → [Alloc C] → [Free D] → [Free E] → [Epilogue]
             (사용중)    (비었음)   (사용중)    (비었음)    (비었음)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🔗 FREE LIST (논리적 세계 - free 블록만!)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

free_listp
    ↓
[Free E] ⟷ [Free D] ⟷ [Free B] → NULL

※ Free B, D, E는 heap에서는 떨어져 있지만,
   free list에서는 연결되어 있어요!
```

## 2️⃣ **각 블록의 내부 구조**

### 📦 **Allocated Block** (할당된 블록)
```
+------------------+
| Header: size|1   | ← alloc bit = 1
+------------------+
|                  |
|   Payload        | ← 사용자 데이터
|   (사용중)       |
|                  |
+------------------+
| Footer: size|1   |
+------------------+
```

### 📭 **Free Block** (빈 블록)
```
+------------------+
| Header: size|0   | ← alloc bit = 0
+------------------+
| Next Free ptr    | ← 다음 FREE 블록 주소 (free list)
+------------------+
| Prev Free ptr    | ← 이전 FREE 블록 주소 (free list)
+------------------+
| (unused space)   |
+------------------+
| Footer: size|0   |
+------------------+
```

## 3️⃣ **주요 동작 3가지**

### 🟢 **MALLOC** (메모리 할당)
```
1. first_fit() → free list를 순회하며 적당한 블록 찾기
   free_listp → Free E → Free D → Free B → NULL
                           ✓ 여기!

2. place() → 찾은 블록을 할당
   - remove_from_free_list(Free D)  // free list에서 제거
   - Header/Footer alloc bit을 1로
   - 남은 공간 있으면 split → add_to_free_list()
```

### 🔴 **FREE** (메모리 해제)
```
1. Header/Footer의 alloc bit을 0으로 변경

2. coalesce() → 인접한 free 블록들과 합치기
   물리적으로 체크:
   [Free?] [현재] [Free?]

   합칠 블록들을 free list에서 제거하고,
   큰 블록 하나로 합쳐서 다시 free list에 추가
```

### 🔵 **COALESCE** (병합)
```
Before:
Heap:      [Alloc] [Free A] [Free B] [Alloc]
Free list: Free C ⟷ Free A ⟷ Free B ⟷ ...

After coalesce:
Heap:      [Alloc] [Free AB (합쳐짐)] [Alloc]
Free list: Free C ⟷ Free AB ⟷ ...
```

## 4️⃣ **핵심 함수들의 역할**

```
┌─────────────────────────────────────────┐
│ mm_init()                               │
│ → heap 초기화, free_listp = NULL       │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│ mm_malloc(size)                         │
│ 1. first_fit(size) → 적당한 블록 찾기  │
│ 2. 없으면 extend_heap() → heap 확장    │
│ 3. place() → 할당                       │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│ place(bp, size)                         │
│ 1. remove_from_free_list(bp)           │
│ 2. 블록 split (필요시)                 │
│ 3. 남은 부분 add_to_free_list()        │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│ mm_free(ptr)                            │
│ 1. alloc bit → 0                        │
│ 2. coalesce(ptr) → 인접 블록 합치기    │
└─────────────────────────────────────────┘
              ↓
┌─────────────────────────────────────────┐
│ coalesce(bp)                            │
│ 1. 인접 블록 체크 (물리적)             │
│ 2. free 블록들 free list에서 제거      │
│ 3. 합친 후 다시 add_to_free_list()     │
└─────────────────────────────────────────┘
```

## 5️⃣ **가장 중요한 개념!**

### 🔑 **두 가지 포인터의 차이**

```c
// 물리적 이동 (heap에서 옆 블록)
NEXT_BLKP(bp)  // header의 size로 계산
PREV_BLKP(bp)  // footer의 size로 계산

// 논리적 이동 (free list에서 다음 free 블록)
NEXT_FREE(bp)  // bp 안에 저장된 포인터 값
PREV_FREE(bp)  // bp+8 안에 저장된 포인터 값
```

## 6️⃣ **중요한 구현 디테일**

### Minimum Block Size
- **Explicit Free List**: 24 bytes (3*DSIZE)
  - Header: 4 bytes
  - Next pointer: 8 bytes
  - Prev pointer: 8 bytes
  - Footer: 4 bytes

### Split Threshold
```c
if ((csize - asize) >= (3*DSIZE)) {
    // Split the block
}
```

### Free List Insertion
- LIFO (Last In First Out) - 항상 맨 앞에 추가
- 간단하고 빠름

## 📝 **이해 체크리스트**

- [ ] 1. Heap과 Free list는 **별개**다
- [ ] 2. Free block은 **payload 공간에 포인터를 저장**한다
- [ ] 3. `NEXT_BLKP` vs `NEXT_FREE`의 차이
- [ ] 4. `place()`는 free list에서 **제거**한다
- [ ] 5. `coalesce()`는 물리적 인접 블록을 **합친다**

## 🐛 **디버깅 팁**

### 개별 trace 파일 테스트
```bash
./mdriver -f traces/short1-bal.rep
./mdriver -f traces/short2-bal.rep
```

### 특정 trace만 verbose 모드로
```bash
./mdriver -V -f traces/short1-bal.rep
```

### GDB 사용
```bash
gdb ./mdriver
(gdb) run -f traces/short1-bal.rep
(gdb) break mm_malloc
(gdb) print *bp
```

