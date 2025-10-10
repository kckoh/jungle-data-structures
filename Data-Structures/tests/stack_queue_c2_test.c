#include "unity.h"
#include <stdlib.h>
#include "linked_list.h"   // LinkedList, insertNode, removeAllItems
#include "stack.h"         // Stack APIs: push, pop, isEmptyStack, removeAllItemsFromStack, createStackFromLinkedList, removeEvenValues

// -------------------- 테스트 픽스처 --------------------
static LinkedList ll;
static Stack stack_;

static void append_values(LinkedList *list, const int *vals, int n) {
    for (int i = 0; i < n; i++) {
        // 맨 뒤에 추가: index = size
        insertNode(list, list->size, vals[i]);
    }
}

void setUp(void) {
    // LinkedList 초기화
    ll.head = NULL;
    ll.size = 0;

    // Stack 초기화
    removeAllItemsFromStack(&stack_);
}

void tearDown(void) {
    removeAllItems(&ll);
    removeAllItemsFromStack(&stack_);
}

// -------------------- createStackFromLinkedList 테스트 --------------------

// 1) 기본 동작: ll = [1,3,5,6,7] -> stack top..bottom = 7,6,5,3,1
void test_createStack_basic(void) {
    const int src[] = {1, 3, 5, 6, 7};
    const int n = (int)(sizeof(src)/sizeof(src[0]));
    append_values(&ll, src, n);

    createStackFromLinkedList(&ll, &stack_);

    const int expected[] = {7, 6, 5, 3, 1}; // pop 순서
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_FALSE_MESSAGE(isEmptyStack(&stack_), "Stack should not be empty yet");
        int got = pop(&stack_);
        TEST_ASSERT_EQUAL_INT_MESSAGE(expected[i], got, "Popped value mismatch");
    }
    TEST_ASSERT_TRUE_MESSAGE(isEmptyStack(&stack_), "Stack should be empty at the end");
}

// 2) 기존 스택 비우기 확인
void test_createStack_clears_existing_stack(void) {
    // 사전 스택에 쓰레기값
    push(&stack_, 42);
    push(&stack_, 99);

    const int src[] = {10, 20, 30};
    const int n = (int)(sizeof(src)/sizeof(src[0]));
    append_values(&ll, src, n);

    createStackFromLinkedList(&ll, &stack_);

    const int expected[] = {30, 20, 10}; // pop 순서
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_FALSE(isEmptyStack(&stack_));
        TEST_ASSERT_EQUAL_INT(expected[i], pop(&stack_));
    }
    TEST_ASSERT_TRUE(isEmptyStack(&stack_));
}

// 3) 빈 리스트 -> 빈 스택
void test_createStack_from_empty_list_results_empty_stack(void) {
    createStackFromLinkedList(&ll, &stack_);
    TEST_ASSERT_TRUE_MESSAGE(isEmptyStack(&stack_), "Stack should remain empty when list is empty");
}

// -------------------- removeEvenValues 테스트 --------------------

// 4) 혼합 값: [1,3,5,6,7] -> 스택(7,6,5,3,1)에서 짝수 6 제거 -> (7,5,3,1)
void test_removeEvenValues_mixed(void) {
    const int src[] = {1, 3, 5, 6, 7};
    const int n = (int)(sizeof(src)/sizeof(src[0]));
    append_values(&ll, src, n);
    createStackFromLinkedList(&ll, &stack_);

    removeEvenValues(&stack_);

    const int expected[] = {7, 5, 3, 1};
    const int m = (int)(sizeof(expected)/sizeof(expected[0]));
    for (int i = 0; i < m; i++) {
        TEST_ASSERT_FALSE(isEmptyStack(&stack_));
        TEST_ASSERT_EQUAL_INT(expected[i], pop(&stack_));
    }
    TEST_ASSERT_TRUE(isEmptyStack(&stack_));
}

// 5) 모두 짝수: [2,4,6,8] -> 제거 후 빈 스택
void test_removeEvenValues_all_evens(void) {
    const int src[] = {2, 4, 6, 8};
    append_values(&ll, src, 4);
    createStackFromLinkedList(&ll, &stack_);

    removeEvenValues(&stack_);

    TEST_ASSERT_TRUE(isEmptyStack(&stack_));
}

// 6) 모두 홀수: [1,3,5] -> 변화 없음 (스택은 5,3,1)
void test_removeEvenValues_all_odds_no_change(void) {
    const int src[] = {1, 3, 5};
    const int n = 3;
    append_values(&ll, src, n);
    createStackFromLinkedList(&ll, &stack_);

    removeEvenValues(&stack_);

    const int expected[] = {5, 3, 1};
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_FALSE(isEmptyStack(&stack_));
        TEST_ASSERT_EQUAL_INT(expected[i], pop(&stack_));
    }
    TEST_ASSERT_TRUE(isEmptyStack(&stack_));
}

// 7) 빈 스택: 호출해도 그대로 빈 상태 유지
void test_removeEvenValues_empty_stack(void) {
    TEST_ASSERT_TRUE(isEmptyStack(&stack_));
    removeEvenValues(&stack_);
    TEST_ASSERT_TRUE(isEmptyStack(&stack_));
}

// -------------------- 테스트 러너 --------------------
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_createStack_basic);
    RUN_TEST(test_createStack_clears_existing_stack);
    RUN_TEST(test_createStack_from_empty_list_results_empty_stack);

    RUN_TEST(test_removeEvenValues_mixed);
    RUN_TEST(test_removeEvenValues_all_evens);
    RUN_TEST(test_removeEvenValues_all_odds_no_change);
    RUN_TEST(test_removeEvenValues_empty_stack);

    return UNITY_END();
}
