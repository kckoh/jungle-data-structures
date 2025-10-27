// test_malloc.c
// Comprehensive correctness suite for malloc lab allocators
// Works with mm.c + memlib.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mm.h"
#include "memlib.h"

/* ==============================================================
   Global counters and colors
   ============================================================== */
static int tests_run = 0;
static int tests_passed = 0;

#define GREEN  "\033[0;32m"
#define RED    "\033[0;31m"
#define YELLOW "\033[0;33m"
#define RESET  "\033[0m"

/* ==============================================================
   Test macros
   ============================================================== */
#define TEST_SETUP()  \
    do { mem_init(); mm_init(); } while(0)

#define TEST(name) \
    printf("\n" YELLOW "TEST: %s" RESET "\n", name); \
    tests_run++; \
    TEST_SETUP();

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf(RED "  ✗ FAIL: %s" RESET "\n", msg); \
            return 0; \
        } \
    } while(0)

#define PASS() \
    do { \
        tests_passed++; \
        printf(GREEN "  ✓ PASS" RESET "\n"); \
        return 1; \
    } while(0)

static int is_aligned(void *p) { return ((size_t)p % 8) == 0; }

/* ==============================================================
   Basic Tests
   ============================================================== */
int test_init() {
    TEST("Initialization");
    PASS();
}

int test_malloc_zero() {
    TEST("malloc(0) returns NULL");
    void *p = mm_malloc(0);
    ASSERT(p == NULL, "malloc(0) must return NULL");
    PASS();
}

int test_single_malloc() {
    TEST("Single malloc");
    void *p = mm_malloc(64);
    ASSERT(p && is_aligned(p), "valid aligned ptr");
    PASS();
}

int test_multiple_malloc() {
    TEST("Multiple malloc");
    void *a = mm_malloc(64);
    void *b = mm_malloc(128);
    void *c = mm_malloc(256);
    ASSERT(a && b && c, "allocations succeeded");
    ASSERT(a != b && b != c && a != c, "unique blocks");
    PASS();
}

int test_malloc_large() {
    TEST("Large malloc (1MB)");
    void *p = mm_malloc(1 << 20);
    ASSERT(p && is_aligned(p), "large allocation aligned");
    PASS();
}

/* ==============================================================
   Free & Coalesce
   ============================================================== */
int test_malloc_free() {
    TEST("malloc + free");
    void *p = mm_malloc(64);
    ASSERT(p, "malloc failed");
    mm_free(p);
    PASS();
}

int test_multiple_free() {
    TEST("multiple malloc/free");
    void *a = mm_malloc(64), *b = mm_malloc(128), *c = mm_malloc(256);
    mm_free(b); mm_free(a); mm_free(c);
    PASS();
}

int test_free_null() {
    TEST("free(NULL)");
    mm_free(NULL);
    PASS();
}

int test_alternating_free_coalesce() {
    TEST("alternating free coalescing");
    void *blk[32];
    for (int i=0;i<32;i++) blk[i]=mm_malloc(64);
    for (int i=0;i<32;i+=2) mm_free(blk[i]);
    for (int i=1;i<32;i+=2) mm_free(blk[i]); // should coalesce full region
    void *big = mm_malloc(2048);
    ASSERT(big != NULL, "coalesced large alloc failed");
    PASS();
}

/* ==============================================================
   Data Integrity & Overlap
   ============================================================== */
int test_write_read() {
    TEST("write/read integrity");
    int *p = mm_malloc(sizeof(int)*10);
    ASSERT(p, "malloc failed");
    for (int i=0;i<10;i++) p[i]=i*7;
    for (int i=0;i<10;i++) ASSERT(p[i]==i*7,"data corrupted");
    mm_free(p);
    PASS();
}

int test_no_overlap() {
    TEST("no overwrite between blocks");
    int *a = mm_malloc(sizeof(int)*100);
    int *b = mm_malloc(sizeof(int)*100);
    for (int i=0;i<100;i++) a[i]=i;
    for (int i=0;i<100;i++) b[i]=i*2;
    for (int i=0;i<100;i++) ASSERT(a[i]==i,"arr1 corrupted");
    for (int i=0;i<100;i++) ASSERT(b[i]==i*2,"arr2 corrupted");
    mm_free(a); mm_free(b);
    PASS();
}

int test_payload_non_overlap_stress() {
    TEST("payload overlap stress");
    enum {N=200};
    void* p[N];
    for(int i=0;i<N;i++) p[i]=mm_malloc((i%5+1)*32);
    for(int i=0;i<N;i++) ASSERT(p[i],"malloc fail");
    for(int i=0;i<N;i++) memset(p[i],(char)(i&0xFF),8);
    for(int i=0;i<N;i+=3) mm_free(p[i]);
    for(int i=1;i<N;i+=2)
        ASSERT(((unsigned char*)p[i])[0]==(unsigned char)(i&0xFF),
               "payload overlap detected");
    PASS();
}

/* ==============================================================
   Realloc
   ============================================================== */
int test_realloc_preserves_data() {
    TEST("realloc preserves data");
    size_t n=128;
    unsigned char* p=mm_malloc(n);
    ASSERT(p,"malloc fail");
    for(size_t i=0;i<n;i++) p[i]=(unsigned char)(i^0xAA);
    unsigned char* q=mm_realloc(p,n*4);
    ASSERT(q,"realloc fail");
    for(size_t i=0;i<n;i++)
        ASSERT(q[i]==(unsigned char)(i^0xAA),"data corrupted after realloc");
    mm_free(q);
    PASS();
}

/* ==============================================================
   Alignment
   ============================================================== */
int test_alignment() {
    TEST("alignment check");
    size_t s[]={1,7,13,19,31,63,127};
    for(int i=0;i<7;i++){
        void* p=mm_malloc(s[i]);
        ASSERT(p && is_aligned(p),"alignment fail");
        mm_free(p);
    }
    PASS();
}

/* ==============================================================
   Summary
   ============================================================== */
int main() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf(  "║      Segregated List Unit Tests        ║\n");
    printf(  "╚════════════════════════════════════════╝\n");

    test_init();
    test_malloc_zero();
    test_single_malloc();
    test_multiple_malloc();
    test_malloc_large();

    test_malloc_free();
    test_multiple_free();
    test_free_null();
    test_alternating_free_coalesce();

    test_write_read();
    test_no_overlap();
    test_payload_non_overlap_stress();

    test_realloc_preserves_data();
    test_alignment();

    printf("\n╔════════════════════════════════════════╗\n");
    printf(  "║              TEST SUMMARY              ║\n");
    printf(  "╠════════════════════════════════════════╣\n");
    printf(  "║  Total:  %3d    Passed: %3d  Failed: %3d ║\n",
            tests_run, tests_passed, tests_run-tests_passed);
    printf(  "╚════════════════════════════════════════╝\n");

    if (tests_passed==tests_run)
        printf(GREEN "\n★ ALL TESTS PASSED ★\n\n" RESET);
    else
        printf(RED "\n✗ SOME TESTS FAILED\n\n" RESET);

    return (tests_passed==tests_run)?0:1;
}
