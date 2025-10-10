// tests/q1_queue_tests.c
#include "unity.h"
#include "queue.h"
#include "linked_list.h"
#include <stdlib.h>

/* ---------- Fixtures & helpers ---------- */
static LinkedList ll;
static Queue q;

static void ll_from_array(LinkedList *list, const int *arr, int n) {
    for (int i = 0; i < n; i++) insertNode(list, list->size, arr[i]);
}

static void assert_ll_equals(LinkedList *list, const int *arr, int n) {
    TEST_ASSERT_EQUAL_INT(n, list->size);
    ListNode *cur = list->head;
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_NOT_NULL(cur);
        TEST_ASSERT_EQUAL_INT(arr[i], cur->item);
        cur = cur->next;
    }
    TEST_ASSERT_NULL(cur);
}

static void assert_queue_equals(Queue *queue, const int *arr, int n) {
    TEST_ASSERT_EQUAL_INT(n, queue->ll.size);
    ListNode *cur = queue->ll.head;
    for (int i = 0; i < n; i++) {
        TEST_ASSERT_NOT_NULL(cur);
        TEST_ASSERT_EQUAL_INT(arr[i], cur->item);
        cur = cur->next;
    }
    TEST_ASSERT_NULL(cur);
}

void setUp(void) {
    ll.head = NULL; ll.size = 0;
    q.ll.head = NULL; q.ll.size = 0;
}

void tearDown(void) {
    removeAllItems(&ll);
    removeAllItemsFromQueue(&q);
}

/* ---------- Tests: createQueueFromLinkedList ---------- */

// list 1..5 -> queue 1..5; ll must remain unchanged
void test_createQueue_basic(void) {
    const int src[] = {1,2,3,4,5};
    ll_from_array(&ll, src, 5);

    createQueueFromLinkedList(&ll, &q);

    assert_queue_equals(&q, src, 5);
    assert_ll_equals(&ll, src, 5);
}

// prefilled queue gets cleared first
void test_createQueue_clears_existing_queue(void) {
    enqueue(&q, 99);
    enqueue(&q, 100);

    const int src[] = {7,8,9};
    ll_from_array(&ll, src, 3);

    createQueueFromLinkedList(&ll, &q);

    assert_queue_equals(&q, src, 3);
}

// empty list -> empty queue (also clears any prefill)
void test_createQueue_from_empty_list_results_empty_queue(void) {
    enqueue(&q, 42); // ensure clear happens

    createQueueFromLinkedList(&ll, &q);

    TEST_ASSERT_TRUE(isEmptyQueue(&q));
    TEST_ASSERT_EQUAL_INT(0, q.ll.size);
    TEST_ASSERT_NULL(q.ll.head);
}

/* ---------- Tests: removeOddValues ---------- */

// mixed -> only evens remain, order preserved
void test_removeOddValues_mixed(void) {
    const int src[]    = {1,2,3,4,5};
    const int expect[] = {2,4};

    for (int i = 0; i < 5; i++) enqueue(&q, src[i]);

    removeOddValues(&q);

    assert_queue_equals(&q, expect, 2);
}

// all odds -> queue becomes empty
void test_removeOddValues_all_odds(void) {
    const int src[] = {1,3,5,7};
    for (int i = 0; i < 4; i++) enqueue(&q, src[i]);

    removeOddValues(&q);

    TEST_ASSERT_TRUE(isEmptyQueue(&q));
}

// all evens -> queue unchanged
void test_removeOddValues_all_evens_no_change(void) {
    const int src[] = {2,4,6,8};
    for (int i = 0; i < 4; i++) enqueue(&q, src[i]);

    removeOddValues(&q);

    assert_queue_equals(&q, src, 4);
}

// empty queue -> no-op
void test_removeOddValues_empty_queue(void) {
    removeOddValues(&q);
    TEST_ASSERT_TRUE(isEmptyQueue(&q));
}

/* ---------- Runner ---------- */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_createQueue_basic);
    RUN_TEST(test_createQueue_clears_existing_queue);
    RUN_TEST(test_createQueue_from_empty_list_results_empty_queue);
    RUN_TEST(test_removeOddValues_mixed);
    RUN_TEST(test_removeOddValues_all_odds);
    RUN_TEST(test_removeOddValues_all_evens_no_change);
    RUN_TEST(test_removeOddValues_empty_queue);
    return UNITY_END();
}
