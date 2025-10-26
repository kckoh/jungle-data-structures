// test_malloc_v2.c - CLEANER VERSION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mm.h"
#include "memlib.h"

// Test counter
static int tests_run = 0;
static int tests_passed = 0;

// Color codes
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"

// Enhanced test macros with automatic setup
#define TEST_SETUP() \
    do { \
        mem_init(); \
        mm_init(); \
    } while(0)

#define TEST(name) \
    printf("\n" YELLOW "TEST: %s" RESET "\n", name); \
    tests_run++; \
    TEST_SETUP()  // ← Automatic setup!

#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf(RED "  ✗ FAIL: %s" RESET "\n", message); \
            return 0; \
        } \
    } while(0)

#define PASS() \
    do { \
        tests_passed++; \
        printf(GREEN "  ✓ PASS" RESET "\n"); \
        return 1; \
    } while(0)

// Helper
int is_valid_pointer(void *ptr) {
    return ptr != NULL && ((size_t)ptr % 8) == 0;
}

/* ========================================
   TESTS - Now much simpler!
======================================== */

int test_init() {
    TEST("Initialization");
    // mem_init() and mm_init() already called by TEST macro!
    PASS();
}

int test_single_malloc() {
    TEST("Single malloc");
    // Already initialized!

    void *ptr = mm_malloc(64);
    ASSERT(ptr != NULL, "malloc should return non-NULL pointer");
    ASSERT(is_valid_pointer(ptr), "pointer should be 8-byte aligned");

    PASS();
}

int test_multiple_malloc() {
    TEST("Multiple malloc");

    void *ptr1 = mm_malloc(64);
    void *ptr2 = mm_malloc(128);
    void *ptr3 = mm_malloc(256);

    ASSERT(ptr1 != NULL, "first malloc failed");
    ASSERT(ptr2 != NULL, "second malloc failed");
    ASSERT(ptr3 != NULL, "third malloc failed");

    ASSERT(ptr1 != ptr2, "pointers should be different");
    ASSERT(ptr2 != ptr3, "pointers should be different");
    ASSERT(ptr1 != ptr3, "pointers should be different");

    PASS();
}

int test_malloc_zero() {
    TEST("Malloc with size 0");

    void *ptr = mm_malloc(0);
    printf("  → malloc(0) returned: %p\n", ptr);

    PASS();
}

int test_malloc_large() {
    TEST("Large malloc");

    void *ptr = mm_malloc(1024 * 1024); // 1 MB
    ASSERT(ptr != NULL, "large malloc should succeed");
    ASSERT(is_valid_pointer(ptr), "large pointer should be aligned");

    PASS();
}

int test_malloc_free() {
    TEST("Malloc and free");

    void *ptr = mm_malloc(64);
    ASSERT(ptr != NULL, "malloc failed");

    mm_free(ptr);  // Should not crash

    PASS();
}

int test_multiple_free() {
    TEST("Multiple malloc and free");

    void *ptr1 = mm_malloc(64);
    void *ptr2 = mm_malloc(128);
    void *ptr3 = mm_malloc(256);

    mm_free(ptr2);
    mm_free(ptr1);
    mm_free(ptr3);

    PASS();
}

int test_free_null() {
    TEST("Free NULL pointer");

    mm_free(NULL);  // Should not crash

    PASS();
}

int test_malloc_after_free() {
    TEST("Malloc after free (reuse)");

    void *ptr1 = mm_malloc(64);
    ASSERT(ptr1 != NULL, "first malloc failed");

    mm_free(ptr1);

    void *ptr2 = mm_malloc(64);
    ASSERT(ptr2 != NULL, "malloc after free failed");

    if (ptr1 == ptr2) {
        printf("  → Block reused (optimal!)\n");
    }

    PASS();
}

int test_write_read() {
    TEST("Write and read data");

    int *ptr = (int *)mm_malloc(sizeof(int) * 10);
    ASSERT(ptr != NULL, "malloc failed");

    // Write
    for (int i = 0; i < 10; i++) {
        ptr[i] = i * 100;
    }

    // Read and verify
    for (int i = 0; i < 10; i++) {
        ASSERT(ptr[i] == i * 100, "data corruption detected");
    }

    mm_free(ptr);

    PASS();
}

int test_no_overwrite() {
    TEST("No overwrite between blocks");

    int *arr1 = (int *)mm_malloc(sizeof(int) * 100);
    int *arr2 = (int *)mm_malloc(sizeof(int) * 100);

    ASSERT(arr1 != NULL && arr2 != NULL, "malloc failed");

    // Fill first
    for (int i = 0; i < 100; i++) {
        arr1[i] = i;
    }

    // Fill second
    for (int i = 0; i < 100; i++) {
        arr2[i] = i * 2;
    }

    // Verify first not corrupted
    for (int i = 0; i < 100; i++) {
        ASSERT(arr1[i] == i, "arr1 was overwritten!");
    }

    // Verify second
    for (int i = 0; i < 100; i++) {
        ASSERT(arr2[i] == i * 2, "arr2 was corrupted!");
    }

    mm_free(arr1);
    mm_free(arr2);

    PASS();
}

int test_coalesce_forward() {
    TEST("Coalesce with next block");

    void *ptr1 = mm_malloc(64);
    void *ptr2 = mm_malloc(64);
    void *ptr3 = mm_malloc(64);

    mm_free(ptr2);
    mm_free(ptr3);

    void *ptr4 = mm_malloc(100);
    ASSERT(ptr4 != NULL, "coalesced allocation failed");

    PASS();
}

int test_alignment() {
    TEST("Alignment check");

    size_t sizes[] = {1, 7, 13, 19, 31, 63, 127};

    for (int i = 0; i < 7; i++) {
        void *ptr = mm_malloc(sizes[i]);
        ASSERT(ptr != NULL, "malloc failed");
        ASSERT(((size_t)ptr % 8) == 0, "pointer not 8-byte aligned");
        mm_free(ptr);
    }

    PASS();
}

/* ========================================
   MAIN
======================================== */

int main() {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║   Segregated Free List - Unit Tests   ║\n");
    printf("╔════════════════════════════════════════╗\n");

    // Basic tests
    printf("\n" YELLOW "━━━ CATEGORY 1: Basic Functionality ━━━" RESET "\n");
    test_init();
    test_single_malloc();
    test_multiple_malloc();
    test_malloc_zero();
    test_malloc_large();

    // Free tests
    printf("\n" YELLOW "━━━ CATEGORY 2: Free Operations ━━━" RESET "\n");
    test_malloc_free();
    test_multiple_free();
    test_free_null();

    // Reuse tests
    printf("\n" YELLOW "━━━ CATEGORY 3: Reuse ━━━" RESET "\n");
    test_malloc_after_free();

    // Data integrity
    printf("\n" YELLOW "━━━ CATEGORY 4: Data Integrity ━━━" RESET "\n");
    test_write_read();
    test_no_overwrite();

    // Coalescing
    printf("\n" YELLOW "━━━ CATEGORY 5: Coalescing ━━━" RESET "\n");
    test_coalesce_forward();

    // Edge cases
    printf("\n" YELLOW "━━━ CATEGORY 6: Edge Cases ━━━" RESET "\n");
    test_alignment();

    // Summary
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║            TEST SUMMARY                ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║  Total Tests:  %3d                     ║\n", tests_run);
    printf("║  Passed:       %3d                     ║\n", tests_passed);
    printf("║  Failed:       %3d                     ║\n", tests_run - tests_passed);
    printf("╚════════════════════════════════════════╝\n");

    if (tests_passed == tests_run) {
        printf("\n" GREEN "★ ALL TESTS PASSED! ★" RESET "\n\n");
        return 0;
    } else {
        printf("\n" RED "✗ SOME TESTS FAILED" RESET "\n\n");
        return 1;
    }
}
