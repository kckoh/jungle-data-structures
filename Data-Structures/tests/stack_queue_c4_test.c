#include "unity.h"
#include <stdlib.h>
#include "queue.h"   // your Queue struct, enqueue, dequeue, etc.
#include "stack.h"   // your Stack struct, push, pop, etc.

/* Global test queue */
Queue testQ;

void setUp(void) {
    // Initialize queue
    testQ.ll.head = NULL;
    testQ.ll.size = 0;
}

void tearDown(void) {
    // Clean up queue after each test
    removeAllItemsFromQueue(&testQ);
}

/* ==================== Test Cases ==================== */

// Helper: fill queue with array of items
void fillQueue(Queue *q, int arr[], int n) {
    for (int i = 0; i < n; i++) {
        enqueue(q, arr[i]);
    }
}

// Helper: convert queue to array for comparison
void queueToArray(Queue *q, int arr[], int n) {
    ListNode *cur = q->ll.head;
    for (int i = 0; i < n && cur != NULL; i++) {
        arr[i] = cur->item;
        cur = cur->next;
    }
}

void test_reverseQueue_normal(void) {
    int input[] = {1, 2, 3, 4, 5};
    int expected[] = {5, 4, 3, 2, 1};
    int actual[5];

    fillQueue(&testQ, input, 5);

    reverse(&testQ);

    queueToArray(&testQ, actual, 5);

    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 5);
}

void test_reverseQueue_singleElement(void) {
    int input[] = {42};
    int expected[] = {42};
    int actual[1];

    fillQueue(&testQ, input, 1);

    reverse(&testQ);

    queueToArray(&testQ, actual, 1);

    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 1);
}

void test_reverseQueue_empty(void) {
    // Reversing empty queue should remain empty
    reverse(&testQ);
    TEST_ASSERT_TRUE(isEmptyQueue(&testQ));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_reverseQueue_normal);
    RUN_TEST(test_reverseQueue_singleElement);
    RUN_TEST(test_reverseQueue_empty);

    return UNITY_END();
}
