#include "linked_list.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdlib.h>



// 테스트용 리스트
LinkedList testList;

// setUp: 각 테스트 시작 전에 리스트 초기화
void setUp(void) {
    testList.head = NULL;
    testList.size = 0;
}

// tearDown: 각 테스트 종료 후 메모리 해제
void tearDown(void) {
    removeAllItems(&testList);
}

// ========== 테스트 케이스들 ==========

// Test1: 입력: 2, 3, 4, 7, 15, 18 → 출력: 3 7 15 2 4 18
void test_moveEvenItemsToBackLL_case1(void) {
    insertNode(&testList, 0, 2);
    insertNode(&testList, 1, 3);
    insertNode(&testList, 2, 4);
    insertNode(&testList, 3, 7);
    insertNode(&testList, 4, 15);
    insertNode(&testList, 5, 18);

    moveEvenItemsToBackLL(&testList);

    int expected[] = {3, 7, 15, 2, 4, 18};
    int *actual = returnList(&testList, 6);

    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 6);
    free(actual);
}

// Test2: 입력: 2, 7, 18, 3, 4, 15 → 출력: 7 3 15 2 18 4
void test_moveEvenItemsToBackLL_case2(void) {
    insertNode(&testList, 0, 2);
    insertNode(&testList, 1, 7);
    insertNode(&testList, 2, 18);
    insertNode(&testList, 3, 3);
    insertNode(&testList, 4, 4);
    insertNode(&testList, 5, 15);

    moveEvenItemsToBackLL(&testList);

    int expected[] = {7, 3, 15, 2, 18, 4};
    int *actual = returnList(&testList, 6);

    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 6);
    free(actual);
}

// Test3: 입력: 1, 3, 5 → 출력: 1 3 5
void test_moveEvenItemsToBackLL_case3(void) {
    insertNode(&testList, 0, 1);
    insertNode(&testList, 1, 3);
    insertNode(&testList, 2, 5);

    moveEvenItemsToBackLL(&testList);

    int expected[] = {1, 3, 5};
    int *actual = returnList(&testList, 3);

    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 3);
    free(actual);
}

// Test4: 입력: 2, 4, 6 → 출력: 2 4 6
void test_moveEvenItemsToBackLL_case4(void) {
    insertNode(&testList, 0, 2);
    insertNode(&testList, 1, 4);
    insertNode(&testList, 2, 6);

    moveEvenItemsToBackLL(&testList);

    int expected[] = {2, 4, 6};
    int *actual = returnList(&testList, 3);

    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 3);
    free(actual);
}

// 메인 실행
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_moveEvenItemsToBackLL_case1);
    RUN_TEST(test_moveEvenItemsToBackLL_case2);
    RUN_TEST(test_moveEvenItemsToBackLL_case3);
    RUN_TEST(test_moveEvenItemsToBackLL_case4);
    return UNITY_END();
}
