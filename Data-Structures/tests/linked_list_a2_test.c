#include "unity.h"
#include <stdlib.h>
#include "linked_list.h"
#include "unity_internals.h"

// Unity setUp/tearDown
LinkedList testList1;
LinkedList testList2;

void setUp(void)
{
    // 각 테스트 전에 리스트 초기화
    testList1.head = NULL;
    testList1.size = 0;

    testList2.head = NULL;
    testList1.size = 0;
}

void tearDown(void)
{
    // 각 테스트 후에 메모리 정리
    removeAllItems(&testList1);
    removeAllItems(&testList2);
}



// ==================== 테스트 케이스 ====================

// Q2
// input:
// LinkedList1: 1, 2, 3
// LinkedList2: 4, 5, 6, 7
// output:
// LinkedList1: 1, 4, 2, 5, 3, 6
// LinkedList2: 7
void test_insert_two_linkedlist_alterante1(void){

    insertNode(&testList1, 0, 1);
    insertNode(&testList1, 1, 2);
    insertNode(&testList1, 2, 3);

    insertNode(&testList2, 0, 4);
    insertNode(&testList2, 1, 5);
    insertNode(&testList2, 2, 6);
    insertNode(&testList2, 3, 7);

    printList(&testList1);
    printList(&testList2);
    printf("AFTER alternateMergeLinkedList \n");
    alternateMergeLinkedList(&testList1,&testList2);

    printList(&testList1);
    printList(&testList2);
    int expected1[6] = {1,4,2,5,3,6};
    int expected2[1] = {7};

    int *actual1 = returnList(&testList1,6);
    int *actual2 = returnList(&testList2,1);


    TEST_ASSERT_EQUAL_INT_ARRAY(expected1, actual1, 6);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected2, actual2, 1);

    free(actual1);
    free(actual2);
}


// input:
// LinkedList1: 1, 5, 7, 3, 9, 11
// LinkedList2: 6, 10, 2, 4
// output:
// LinkedList1: 1, 6, 5, 10, 7, 2, 3, 4, 9, 11
// LinkedList2: empty
void test_insert_two_linkedlist_alterante2(void){

    insertNode(&testList1, 0, 1);
    insertNode(&testList1, 1, 5);
    insertNode(&testList1, 2, 7);
    insertNode(&testList1, 3, 3);
    insertNode(&testList1, 4, 9);
    insertNode(&testList1, 5, 11);

    insertNode(&testList2, 0, 6);
    insertNode(&testList2, 1, 10);
    insertNode(&testList2, 2, 2);
    insertNode(&testList2, 3, 4);

    alternateMergeLinkedList(&testList1,&testList2);
    printList(&testList1);
    printList(&testList2);
    int expected1[10] = {1, 6, 5, 10, 7, 2, 3, 4, 9, 11};
    //int expected2[0] = {};

    int *actual1 = returnList(&testList1,10);
    int *actual2 = returnList(&testList2,0);

    TEST_ASSERT_EQUAL_INT_ARRAY(expected1, actual1, 10);
    //TEST_ASSERT_EQUAL_INT_ARRAY(expected2, actual2, 0);
    TEST_ASSERT_NULL(actual2);

    free(actual1);
    free(actual2);
}


int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_insert_two_linkedlist_alterante1);
    RUN_TEST(test_insert_two_linkedlist_alterante2);

    return UNITY_END();
}
