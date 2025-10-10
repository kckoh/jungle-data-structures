#include "unity.h"
#include "linked_list.h"
#include <stdlib.h>

// Helper: create a list from array
void createListFromArray(LinkedList *ll, int arr[], int size) {
    ll->head = NULL;
    ll->size = 0;
    for (int i = 0; i < size; i++) {
        insertNode(ll, i, arr[i]);
    }
}

// Helper: verify list content matches expected
void verifyList(LinkedList *ll, int expected[], int size) {
    TEST_ASSERT_EQUAL_INT(size, ll->size);
    ListNode *cur = ll->head;
    for (int i = 0; i < size; i++) {
        TEST_ASSERT_NOT_NULL(cur);
        TEST_ASSERT_EQUAL_INT(expected[i], cur->item);
        cur = cur->next;
    }
    TEST_ASSERT_NULL(cur);
}

void setUp(void) {}
void tearDown(void) {}

// === Test Cases ===

// Test 1: normal case
void test_recursiveReverse_basic(void) {
    LinkedList ll;
    int input[] = {1, 2, 3, 4, 5};
    int expected[] = {5, 4, 3, 2, 1};
    createListFromArray(&ll, input, 5);

    recursiveReverse(&ll.head);

    verifyList(&ll, expected, 5);
    removeAllItems(&ll);
}

// Test 2: single node
void test_recursiveReverse_singleNode(void) {
    LinkedList ll;
    int input[] = {42};
    int expected[] = {42};
    createListFromArray(&ll, input, 1);

    recursiveReverse(&ll.head);

    verifyList(&ll, expected, 1);
    removeAllItems(&ll);
}

// Test 3: empty list
void test_recursiveReverse_emptyList(void) {
    ListNode *head = NULL;
    recursiveReverse(&head);
    TEST_ASSERT_NULL(head);
}

// Test 4: two nodes
void test_recursiveReverse_twoNodes(void) {
    LinkedList ll;
    int input[] = {10, 20};
    int expected[] = {20, 10};
    createListFromArray(&ll, input, 2);

    recursiveReverse(&ll.head);

    verifyList(&ll, expected, 2);
    removeAllItems(&ll);
}

// Test 5: list with negative numbers
void test_recursiveReverse_negativeNumbers(void) {
    LinkedList ll;
    int input[] = {-1, -2, -3};
    int expected[] = {-3, -2, -1};
    createListFromArray(&ll, input, 3);

    recursiveReverse(&ll.head);

    verifyList(&ll, expected, 3);
    removeAllItems(&ll);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_recursiveReverse_basic);
    RUN_TEST(test_recursiveReverse_singleNode);
    RUN_TEST(test_recursiveReverse_emptyList);
    RUN_TEST(test_recursiveReverse_twoNodes);
    RUN_TEST(test_recursiveReverse_negativeNumbers);
    return UNITY_END();
}
