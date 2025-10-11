#include "unity.h"
#include <stdlib.h>
#include "stack.h"

Stack testStack;

void setUp(void)
{
    testStack.ll.head = NULL;
    testStack.ll.size = 0;
}

void tearDown(void)
{
    removeAllItemsFromStack(&testStack);
}

// Helper: push array to stack (topmost element is leftmost)
void pushArray(Stack *s, int *arr, int size)
{
    for (int i = size - 1; i >= 0; i--)
        push(s, arr[i]);
}

// Helper: return array from stack (topmost leftmost)
void getStackArray(Stack *s, int *out)
{
    ListNode *cur = s->ll.head;
    int i = 0;
    while (cur != NULL)
    {
        out[i++] = cur->item;
        cur = cur->next;
    }
}

// ==================== TEST CASES ====================

// (1,2,3,4,5,6,7), value=4 → (4,5,6,7)
void test_removeUntilStack_case1(void)
{
    int input[] = {1, 2, 3, 4, 5, 6, 7};
    pushArray(&testStack, input, 7);

    removeUntilStack(&testStack, 4);

    int expected[] = {4, 5, 6, 7};
    int actual[4];
    getStackArray(&testStack, actual);

    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 4);
}

// (10,20,15,25,5), value=15 → (15,25,5)
void test_removeUntilStack_case2(void)
{
    int input[] = {10, 20, 15, 25, 5};
    pushArray(&testStack, input, 5);

    removeUntilStack(&testStack, 15);

    int expected[] = {15, 25, 5};
    int actual[3];
    getStackArray(&testStack, actual);

    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 3);
}

// No match: (1,2,3), value=9 → empty
void test_removeUntilStack_no_match(void)
{
    int input[] = {1, 2, 3};
    pushArray(&testStack, input, 3);

    removeUntilStack(&testStack, 9);

    TEST_ASSERT_TRUE(isEmptyStack(&testStack));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_removeUntilStack_case1);
    RUN_TEST(test_removeUntilStack_case2);
    RUN_TEST(test_removeUntilStack_no_match);
    return UNITY_END();
}
