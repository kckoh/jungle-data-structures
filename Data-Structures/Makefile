# 컴파일러 및 플래그 설정
CC = gcc
CFLAGS = -I. -I./tests -Wall -g

# 테스트 프레임워크 소스
UNITY = tests/unity.c

# 공통 구현 파일
SRCS_LINKED_LIST = tests/linked_list.c
SRCS_QUEUE = tests/queue.c
SRCS_STACK = tests/stack.c
SRCS_BINARY_TREE = tests/binary_tree.c

# 테스트 소스 파일들 자동 감지
TEST_SOURCES_A := $(wildcard tests/linked_list_a*_test.c)
TEST_SOURCES_C := $(wildcard tests/stack_queue_c*_test.c)
TEST_SOURCES_E := $(wildcard tests/binary_tree_e*_test.c)

# 모든 테스트 소스 파일 목록을 통합
# function name duplicates between trees and stack; don't run this
ALL_TEST_SOURCES := $(TEST_SOURCES_A) $(TEST_SOURCES_C) $(TEST_SOURCES_E)

# 소스 파일 이름으로부터 타겟(실행 파일) 이름 생성
TARGET_A_STEMS := $(patsubst tests/%_test.c,%,$(TEST_SOURCES_A))
TARGET_C_STEMS := $(patsubst tests/%_test.c,%,$(TEST_SOURCES_C))
TARGET_E_STEMS := $(patsubst tests/%_test.c,%,$(TEST_SOURCES_E))

TARGET_A_RUNNERS := $(addprefix test_runner_,$(TARGET_A_STEMS))
TARGET_C_RUNNERS := $(addprefix test_runner_,$(TARGET_C_STEMS))
TARGET_E_RUNNERS := $(addprefix test_runner_,$(TARGET_E_STEMS))

# 모든 타겟 실행 파일
ALL_TARGETS := $(TARGET_A_RUNNERS) $(TARGET_C_RUNNERS) $(TARGET_E_RUNNERS)
.SECONDARY:
#-----------------------------------------------------------------------------
# 규칙 정의
#-----------------------------------------------------------------------------



# 'test' 타겟: 모든 테스트를 순차적으로 실행
.PHONY: test
test: $(ALL_TARGETS)
	@echo "--- Running all tests sequentially ---"
	@for target in $(ALL_TARGETS); do \
		echo "-> Running $$target..."; \
		./$$target; \
	done
	@echo "--- All tests finished! ---"

# 'clean' 타겟: 모든 테스트 실행 파일을 삭제
.PHONY: clean
clean:
	rm -f $(ALL_TARGETS)



# Binary Tree 규칙을 '일반 규칙'보다 위에 둡니다.
test_runner_binary_tree_%_test: tests/binary_tree_%_test.c $(UNITY) $(SRCS_BINARY_TREE)
	$(CC) $(CFLAGS) -o $@ $^


# Queue 규칙을 '일반 규칙'보다 위에 둡니다.
test_runner_stack_queue_%_test: tests/stack_queue_%_test.c $(UNITY) $(SRCS_LINKED_LIST) $(SRCS_QUEUE) $(SRCS_STACK)
	$(CC) $(CFLAGS) -o $@ $^

# ↓ 이 일반 규칙은 아래로 내려둠
test_runner_%_test: tests/%_test.c $(UNITY) $(SRCS_LINKED_LIST)
	$(CC) $(CFLAGS) -o $@ $^

# 특정 테스트를 빌드하고 실행하는 PHONY 타겟
# make run_linked_list_a1_test 명령으로 개별 테스트를 빌드하고 실행
.PHONY: run_%_test
run_%_test: test_runner_%_test
	@echo "-> Running $<..."
	@./$<
