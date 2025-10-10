// File: tests/stack_pairwise_consecutive_test.c
#include "unity.h"
#include "stack.h"        // 프로젝트의 Stack / push / pop / removeAllItems 선언 헤더
// 필요시: #include "linked_list.h"

static Stack s;

/* ---------- Unity Hooks ---------- */
void setUp(void) {
    // 스택 초기화
    s.ll.head = NULL;
    s.ll.size = 0;
}
void tearDown(void) {
    // 스택 메모리 정리
    removeAllItems(&(s.ll));
}

/* ---------- Helpers ---------- */
// arr[0]이 스택의 top이 되도록 push (push가 top에 쌓인다는 가정)
static void push_seq_top_first(Stack *st, const int *arr, int n) {
    for (int i = n - 1; i >= 0; --i) {
        push(st, arr[i]);
    }
}

/* ---------- Tests ---------- */

// (16, 15, 11, 10, 5, 4) => pairwise consecutive => 1
void test_pairwise_consecutive_true_even(void) {
    int a[] = {16, 15, 11, 10, 5, 4};
    push_seq_top_first(&s, a, sizeof(a)/sizeof(a[0]));

    int got = isStackPairwiseConsecutive(&s);
    TEST_ASSERT_EQUAL_INT(1, got);

    // 원본 보존 여부가 과제 요구사항에 없다면 스택 클린만 보장
    // (요구가 있다면 여기에 원복 검사 추가)
}

// (16, 15, 11, 10, 5, 1) => not consecutive => 0
void test_pairwise_consecutive_false_even(void) {
    int a[] = {16, 15, 11, 10, 5, 1};
    push_seq_top_first(&s, a, sizeof(a)/sizeof(a[0]));

    int got = isStackPairwiseConsecutive(&s);
    TEST_ASSERT_EQUAL_INT(0, got);
}

// (16, 15, 11, 10, 5) => odd size => 0
void test_pairwise_consecutive_odd_size(void) {
    int a[] = {16, 15, 11, 10, 5};
    push_seq_top_first(&s, a, sizeof(a)/sizeof(a[0]));

    int got = isStackPairwiseConsecutive(&s);
    TEST_ASSERT_EQUAL_INT(0, got);
}

// (1, 2, 3, 4) => 1-2 OK, 3-4 OK => 1
void test_pairwise_consecutive_simple_true(void) {
    int a[] = {1, 2, 3, 4};
    push_seq_top_first(&s, a, sizeof(a)/sizeof(a[0]));

    int got = isStackPairwiseConsecutive(&s);
    TEST_ASSERT_EQUAL_INT(1, got);
}

// (1, 2, 3, 5) => 3-5 불연속 => 0
void test_pairwise_consecutive_simple_false(void) {
    int a[] = {1, 2, 3, 5};
    push_seq_top_first(&s, a, sizeof(a)/sizeof(a[0]));

    int got = isStackPairwiseConsecutive(&s);
    TEST_ASSERT_EQUAL_INT(0, got);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_pairwise_consecutive_true_even);
    RUN_TEST(test_pairwise_consecutive_false_even);
    RUN_TEST(test_pairwise_consecutive_odd_size);
    RUN_TEST(test_pairwise_consecutive_simple_true);
    RUN_TEST(test_pairwise_consecutive_simple_false);

    return UNITY_END();
}
