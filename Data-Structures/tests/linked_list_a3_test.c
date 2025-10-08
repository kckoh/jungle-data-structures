#include "unity.h"
#include <stdlib.h>
#include "linked_list.h"
#include "unity_internals.h"

// ====== 테스트 픽스처 ======
LinkedList testList1;

void setUp(void)
{
    testList1.head = NULL;
    testList1.size = 0;
}

void tearDown(void)
{
    removeAllItems(&testList1);
}

// ====== 헬퍼 ======
static void buildList(LinkedList *ll, const int *arr, int n)
{
    for (int i = 0; i < n; i++) {
        // 끝에 추가: i 위치에 삽입
        int rc = insertNode(ll, i, arr[i]);
        TEST_ASSERT_NOT_EQUAL_MESSAGE(-1, rc, "insertNode failed");
    }
}

static void assertListEquals(LinkedList *ll, const int *expected, int n)
{
    TEST_ASSERT_EQUAL_INT_MESSAGE(n, ll->size, "size mismatch");

    ListNode *cur = ll->head;
    int i = 0;
    while (i < n && cur) {
        TEST_ASSERT_EQUAL_INT(expected[i], cur->item);
        cur = cur->next;
        i++;
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(n, i, "length mismatch while iterating");
    TEST_ASSERT_NULL_MESSAGE(cur, "list has extra nodes");
}

// ====== 테스트 대상 프로토타입 (prod 코드에 구현되어 있어야 함) ======
// void moveOddItemsToBackLL(LinkedList *ll);

// ====== 케이스 ======
// Test1: 2,3,4,7,15,18 -> 2 4 18 3 7 15
void test_moveOddItemsToBackLL_case1(void)
{
    const int in[]  = {2, 3, 4, 7, 15, 18};
    const int out[] = {2, 4, 18, 3, 7, 15};
    buildList(&testList1, in, sizeof(in)/sizeof(in[0]));

    moveOddItemsToBackLL(&testList1);

    assertListEquals(&testList1, out, sizeof(out)/sizeof(out[0]));
}

// Test2: 2,7,18,3,4,15 -> 2 18 4 7 3 15
void test_moveOddItemsToBackLL_case2(void)
{
    const int in[]  = {2, 7, 18, 3, 4, 15};
    const int out[] = {2, 18, 4, 7, 3, 15};
    buildList(&testList1, in, sizeof(in)/sizeof(in[0]));

    moveOddItemsToBackLL(&testList1);

    assertListEquals(&testList1, out, sizeof(out)/sizeof(out[0]));
}

// Test3: 1,3,5 -> 1 3 5 (모두 홀수: 변화 없음)
void test_moveOddItemsToBackLL_all_odd(void)
{
    const int in[]  = {1, 3, 5};
    const int out[] = {1, 3, 5};
    buildList(&testList1, in, sizeof(in)/sizeof(in[0]));

    moveOddItemsToBackLL(&testList1);

    assertListEquals(&testList1, out, sizeof(out)/sizeof(out[0]));
}

// Test4: 2,4,6 -> 2 4 6 (모두 짝수: 변화 없음)
void test_moveOddItemsToBackLL_all_even(void)
{
    const int in[]  = {2, 4, 6};
    const int out[] = {2, 4, 6};
    buildList(&testList1, in, sizeof(in)/sizeof(in[0]));

    moveOddItemsToBackLL(&testList1);

    assertListEquals(&testList1, out, sizeof(out)/sizeof(out[0]));
}

// Edge: 빈 리스트 -> 빈 리스트
void test_moveOddItemsToBackLL_empty(void)
{
    moveOddItemsToBackLL(&testList1);
    TEST_ASSERT_EQUAL_INT(0, testList1.size);
    TEST_ASSERT_NULL(testList1.head);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_moveOddItemsToBackLL_case1);
    RUN_TEST(test_moveOddItemsToBackLL_case2);
    RUN_TEST(test_moveOddItemsToBackLL_all_odd);
    RUN_TEST(test_moveOddItemsToBackLL_all_even);
    RUN_TEST(test_moveOddItemsToBackLL_empty);

    return UNITY_END();
}
