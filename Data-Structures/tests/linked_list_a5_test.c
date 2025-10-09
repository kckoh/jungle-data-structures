#include "unity.h"
#include <stdlib.h>
#include "linked_list.h"   // LinkedList, ListNode, insertNode, removeAllItems 등
#include "unity_internals.h"

// Test LinkedLists
LinkedList testList;
LinkedList frontList;
LinkedList backList;

void setUp(void) {
    testList.head = NULL;
    testList.size = 0;

    frontList.head = NULL;
    frontList.size = 0;

    backList.head = NULL;
    backList.size = 0;
}

void tearDown(void) {
    removeAllItems(&testList);
    removeAllItems(&frontList);
    removeAllItems(&backList);
}

// Helper: Convert linked list to array for assertion
void listToArray(LinkedList *ll, int *arr) {
    ListNode *cur = ll->head;
    int i = 0;
    while(cur != NULL) {
        arr[i++] = cur->item;
        cur = cur->next;
    }
}

void test_frontBackSplit_even_list(void) {
    // Input: 2, 3, 5, 6
    insertNode(&testList, 0, 2);
    insertNode(&testList, 1, 3);
    insertNode(&testList, 2, 5);
    insertNode(&testList, 3, 6);

    frontBackSplitLL(&testList, &frontList, &backList);

    int expectedFront[] = {2, 3};
    int expectedBack[]  = {5, 6};

    int actualFront[2];
    int actualBack[2];

    listToArray(&frontList, actualFront);
    listToArray(&backList, actualBack);

    TEST_ASSERT_EQUAL_INT_ARRAY(expectedFront, actualFront, 2);
    TEST_ASSERT_EQUAL_INT_ARRAY(expectedBack, actualBack, 2);
}

void test_frontBackSplit_odd_list(void) {
    // Input: 2, 3, 5, 6, 7
    insertNode(&testList, 0, 2);
    insertNode(&testList, 1, 3);
    insertNode(&testList, 2, 5);
    insertNode(&testList, 3, 6);
    insertNode(&testList, 4, 7);

    frontBackSplitLL(&testList, &frontList, &backList);

    int expectedFront[] = {2, 3, 5};
    int expectedBack[]  = {6, 7};

    int actualFront[3];
    int actualBack[2];

    listToArray(&frontList, actualFront);
    listToArray(&backList, actualBack);

    TEST_ASSERT_EQUAL_INT_ARRAY(expectedFront, actualFront, 3);
    TEST_ASSERT_EQUAL_INT_ARRAY(expectedBack, actualBack, 2);
}

void test_frontBackSplit_single_element(void) {
    // Input: 10
    insertNode(&testList, 0, 10);

    frontBackSplitLL(&testList, &frontList, &backList);

    int expectedFront[] = {10};
    int expectedBack[]  = {};

    int actualFront[1];
    listToArray(&frontList, actualFront);

    TEST_ASSERT_EQUAL_INT_ARRAY(expectedFront, actualFront, 1);
    TEST_ASSERT_NULL(backList.head); // back list must be empty
}

void test_frontBackSplit_empty_list(void) {
    // Input: empty
    frontBackSplitLL(&testList, &frontList, &backList);

    TEST_ASSERT_NULL(frontList.head);
    TEST_ASSERT_NULL(backList.head);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_frontBackSplit_even_list);
    RUN_TEST(test_frontBackSplit_odd_list);
    RUN_TEST(test_frontBackSplit_single_element);
    RUN_TEST(test_frontBackSplit_empty_list);

    return UNITY_END();
}
