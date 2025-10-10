#include "unity.h"
#include <stdlib.h>
#include "linked_list.h"
#include "queue.h"    // Queue, enqueue, dequeue, isEmptyQueue, removeAllItemsFromQueue, recursiveReverseQueue

/* ---------- Test Fixture ---------- */
static Queue testQ;

void setUp(void) {
    testQ.ll.head = NULL;
    testQ.ll.size = 0;
}

void tearDown(void) {
    removeAllItemsFromQueue(&testQ);
}

/* ---------- Helpers ---------- */
// q에 배열 원소를 순서대로 enqueue
static void fillQueue(Queue *q, const int *arr, int n) {
    for (int i = 0; i < n; i++) enqueue(q, arr[i]);
}

// q의 내용을 배열로 복사(헤드부터 순서대로)
// 내부 LinkedList를 직접 순회 (테스트 용도)
static void queueToArray(const Queue *q, int *out, int n) {
    ListNode *cur = q->ll.head;
    for (int i = 0; i < n && cur != NULL; i++, cur = cur->next) {
        out[i] = cur->item;
    }
}

/* ---------- Tests ---------- */

// (1,2,3,4,5) -> (5,4,3,2,1)
void test_recursiveReverseQueue_normal(void) {
    int input[]    = {1, 2, 3, 4, 5};
    int expected[] = {5, 4, 3, 2, 1};
    int actual[5];

    fillQueue(&testQ, input, 5);
    TEST_ASSERT_EQUAL_INT(5, testQ.ll.size);

    recursiveReverseQueue(&testQ);

    TEST_ASSERT_EQUAL_INT(5, testQ.ll.size); // 크기 보존
    queueToArray(&testQ, actual, 5);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 5);
}

// 단일 원소 케이스
void test_recursiveReverseQueue_single(void) {
    int input[]    = {42};
    int expected[] = {42};
    int actual[1];

    fillQueue(&testQ, input, 1);
    recursiveReverseQueue(&testQ);

    TEST_ASSERT_FALSE(isEmptyQueue(&testQ));
    TEST_ASSERT_EQUAL_INT(1, testQ.ll.size);
    queueToArray(&testQ, actual, 1);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 1);
}

// 빈 큐 케이스 (변화 없어야 함, 크래시 없어야 함)
void test_recursiveReverseQueue_empty(void) {
    TEST_ASSERT_TRUE(isEmptyQueue(&testQ));
    recursiveReverseQueue(&testQ);
    TEST_ASSERT_TRUE(isEmptyQueue(&testQ));
    TEST_ASSERT_EQUAL_INT(0, testQ.ll.size);
}

// 중복/음수 포함 케이스 (순서만 정확히 뒤집히면 됨)
void test_recursiveReverseQueue_mixedNumbers(void) {
    int input[]    = {-1, -1, 0, 7, 7};
    int expected[] = {7, 7, 0, -1, -1};
    int actual[5];

    fillQueue(&testQ, input, 5);
    recursiveReverseQueue(&testQ);

    TEST_ASSERT_EQUAL_INT(5, testQ.ll.size);
    queueToArray(&testQ, actual, 5);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 5);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_recursiveReverseQueue_normal);
    RUN_TEST(test_recursiveReverseQueue_single);
    RUN_TEST(test_recursiveReverseQueue_empty);
    RUN_TEST(test_recursiveReverseQueue_mixedNumbers);

    return UNITY_END();
}
