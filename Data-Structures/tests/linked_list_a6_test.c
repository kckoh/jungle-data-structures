#include "unity.h"
#include "linked_list.h"
#include <stdlib.h>

void setUp(void) {
    // Setup before each test
}

void tearDown(void) {
    // Cleanup after each test
}

// Helper function to create a linked list from array
void createListFromArray(LinkedList *ll, int arr[], int size) {
    ll->head = NULL;
    ll->size = 0;
    for (int i = 0; i < size; i++) {
        insertNode(ll, i, arr[i]);
    }
}

// Helper function to verify list contents
void verifyList(LinkedList *ll, int expected[], int size) {
    TEST_ASSERT_NOT_NULL(ll);
    TEST_ASSERT_EQUAL_INT(size, ll->size);

    ListNode *cur = ll->head;
    for (int i = 0; i < size; i++) {
        TEST_ASSERT_NOT_NULL_MESSAGE(cur, "List is shorter than expected");
        TEST_ASSERT_EQUAL_INT(expected[i], cur->item);
        cur = cur->next;
    }
    TEST_ASSERT_NULL_MESSAGE(cur, "List is longer than expected");
}

// Test 1: Basic case - max in the middle
void test_moveMaxToFront_maxInMiddle(void) {
    LinkedList ll;
    int input[] = {30, 20, 40, 70, 50};
    createListFromArray(&ll, input, 5);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    int expected[] = {70, 30, 20, 40, 50};
    verifyList(&ll, expected, 5);

    removeAllItems(&ll);
}

// Test 2: Max is already at front
void test_moveMaxToFront_maxAlreadyAtFront(void) {
    LinkedList ll;
    int input[] = {100, 20, 40, 70};
    createListFromArray(&ll, input, 4);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    int expected[] = {100, 20, 40, 70};
    verifyList(&ll, expected, 4);

    removeAllItems(&ll);
}

// Test 3: Max at the end
void test_moveMaxToFront_maxAtEnd(void) {
    LinkedList ll;
    int input[] = {10, 20, 30, 100};
    createListFromArray(&ll, input, 4);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    int expected[] = {100, 10, 20, 30};
    verifyList(&ll, expected, 4);

    removeAllItems(&ll);
}

// Test 4: Single node
void test_moveMaxToFront_singleNode(void) {
    LinkedList ll;
    int input[] = {42};
    createListFromArray(&ll, input, 1);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    int expected[] = {42};
    verifyList(&ll, expected, 1);

    removeAllItems(&ll);
}

// Test 5: Two nodes - max at front
void test_moveMaxToFront_twoNodes_maxAtFront(void) {
    LinkedList ll;
    int input[] = {50, 30};
    createListFromArray(&ll, input, 2);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    int expected[] = {50, 30};
    verifyList(&ll, expected, 2);

    removeAllItems(&ll);
}

// Test 6: Two nodes - max at end
void test_moveMaxToFront_twoNodes_maxAtEnd(void) {
    LinkedList ll;
    int input[] = {30, 50};
    createListFromArray(&ll, input, 2);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    int expected[] = {50, 30};
    verifyList(&ll, expected, 2);

    removeAllItems(&ll);
}

// Test 7: Empty list
void test_moveMaxToFront_emptyList(void) {
    ListNode *head = NULL;

    int result = moveMaxToFront(&head);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_NULL(head);
}

// Test 8: All same values
void test_moveMaxToFront_allSameValues(void) {
    LinkedList ll;
    int input[] = {50, 50, 50};
    createListFromArray(&ll, input, 3);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    int expected[] = {50, 50, 50};
    verifyList(&ll, expected, 3);

    removeAllItems(&ll);
}

// Test 9: Negative numbers
void test_moveMaxToFront_negativeNumbers(void) {
    LinkedList ll;
    int input[] = {-30, -50, -10, -40};
    createListFromArray(&ll, input, 4);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    int expected[] = {-10, -30, -50, -40};
    verifyList(&ll, expected, 4);

    removeAllItems(&ll);
}

// Test 10: Mix of positive and negative
void test_moveMaxToFront_mixedNumbers(void) {
    LinkedList ll;
    int input[] = {-10, 5, -20, 15, -5};
    createListFromArray(&ll, input, 5);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    int expected[] = {15, -10, 5, -20, -5};
    verifyList(&ll, expected, 5);

    removeAllItems(&ll);
}

// Test 11: Multiple max values (first occurrence should move)
void test_moveMaxToFront_multipleMaxValues(void) {
    LinkedList ll;
    int input[] = {30, 70, 40, 70, 50};
    createListFromArray(&ll, input, 5);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    // First occurrence of 70 should move to front
    int expected[] = {70, 30, 40, 70, 50};
    verifyList(&ll, expected, 5);

    removeAllItems(&ll);
}

// Test 12: Large list
void test_moveMaxToFront_largeList(void) {
    LinkedList ll;
    int input[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 5};
    createListFromArray(&ll, input, 11);

    int result = moveMaxToFront(&ll.head);

    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_INT(100, ll.head->item);
    TEST_ASSERT_EQUAL_INT(10, ll.head->next->item);

    removeAllItems(&ll);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_moveMaxToFront_maxInMiddle);
    RUN_TEST(test_moveMaxToFront_maxAlreadyAtFront);
    RUN_TEST(test_moveMaxToFront_maxAtEnd);
    RUN_TEST(test_moveMaxToFront_singleNode);
    RUN_TEST(test_moveMaxToFront_twoNodes_maxAtFront);
    RUN_TEST(test_moveMaxToFront_twoNodes_maxAtEnd);
    RUN_TEST(test_moveMaxToFront_emptyList);
    RUN_TEST(test_moveMaxToFront_allSameValues);
    RUN_TEST(test_moveMaxToFront_negativeNumbers);
    RUN_TEST(test_moveMaxToFront_mixedNumbers);
    RUN_TEST(test_moveMaxToFront_multipleMaxValues);
    RUN_TEST(test_moveMaxToFront_largeList);

    return UNITY_END();
}
