#include "unity.h"
#include <stdlib.h>
#include "linked_list.h"

// Unity setUp/tearDown
LinkedList testList;

void setUp(void)
{
    // 각 테스트 전에 리스트 초기화
    testList.head = NULL;
    testList.size = 0;
}

void tearDown(void)
{
    // 각 테스트 후에 메모리 정리
    removeAllItems(&testList);
}

// 헬퍼 함수: 리스트의 특정 인덱스 값 확인
int getItemAt(LinkedList *ll, int index)
{
    ListNode *node = findNode(ll, index);
    return node ? node->item : -1;
}

// ==================== 테스트 케이스 ====================

// 테스트 1: 빈 리스트에 첫 번째 아이템 삽입
void test_insert_into_empty_list(void)
{
    int result = insertSortedLL(&testList, 5);

    TEST_ASSERT_EQUAL(0, result); // 인덱스 0에 삽입되어야 함
    TEST_ASSERT_EQUAL(1, testList.size);
    TEST_ASSERT_EQUAL(5, testList.head->item);
}

// 테스트 2: 리스트 끝에 삽입 (가장 큰 값)
void test_insert_at_end(void)
{
    insertSortedLL(&testList, 2);
    insertSortedLL(&testList, 3);
    insertSortedLL(&testList, 5);

    int result = insertSortedLL(&testList, 9);

    TEST_ASSERT_EQUAL(3, result); // 인덱스 3에 삽입
    TEST_ASSERT_EQUAL(4, testList.size);
    TEST_ASSERT_EQUAL(9, getItemAt(&testList, 3));
}

// 테스트 3: 리스트 중간에 삽입
void test_insert_in_middle(void)
{
    insertSortedLL(&testList, 2);
    insertSortedLL(&testList, 3);
    insertSortedLL(&testList, 5);
    insertSortedLL(&testList, 7);
    insertSortedLL(&testList, 9);

    int result = insertSortedLL(&testList, 8);

    TEST_ASSERT_EQUAL(4, result); // 인덱스 4에 삽입 (7과 9 사이)
    TEST_ASSERT_EQUAL(6, testList.size);
    TEST_ASSERT_EQUAL(8, getItemAt(&testList, 4));
    TEST_ASSERT_EQUAL(9, getItemAt(&testList, 5));
}

// 테스트 4: 리스트 시작 부분에 삽입 (가장 작은 값)
void test_insert_at_beginning(void)
{
    insertSortedLL(&testList, 5);
    insertSortedLL(&testList, 7);
    insertSortedLL(&testList, 9);

    int result = insertSortedLL(&testList, 1);

    TEST_ASSERT_EQUAL(0, result); // 인덱스 0에 삽입
    TEST_ASSERT_EQUAL(4, testList.size);
    TEST_ASSERT_EQUAL(1, testList.head->item);
}

// 테스트 5: 중복 값 삽입 시도 (실패해야 함)
void test_insert_duplicate_value(void)
{
    insertSortedLL(&testList, 5);
    insertSortedLL(&testList, 7);
    insertSortedLL(&testList, 9);

    int result = insertSortedLL(&testList, 7); // 이미 존재하는 값

    TEST_ASSERT_EQUAL(-1, result);       // 실패 시 -1 반환
    TEST_ASSERT_EQUAL(3, testList.size); // 크기 변하지 않음
}

// 테스트 6: 여러 중복 시도
void test_multiple_duplicates(void)
{
    insertSortedLL(&testList, 2);
    insertSortedLL(&testList, 3);
    insertSortedLL(&testList, 5);

    TEST_ASSERT_EQUAL(-1, insertSortedLL(&testList, 2));
    TEST_ASSERT_EQUAL(-1, insertSortedLL(&testList, 3));
    TEST_ASSERT_EQUAL(-1, insertSortedLL(&testList, 5));
    TEST_ASSERT_EQUAL(3, testList.size);
}

// 테스트 7: 연속된 값 삽입
void test_insert_sequential_values(void)
{
    TEST_ASSERT_EQUAL(0, insertSortedLL(&testList, 2));
    TEST_ASSERT_EQUAL(1, insertSortedLL(&testList, 3));
    TEST_ASSERT_EQUAL(2, insertSortedLL(&testList, 5));
    TEST_ASSERT_EQUAL(3, insertSortedLL(&testList, 7));
    TEST_ASSERT_EQUAL(4, insertSortedLL(&testList, 9));

    TEST_ASSERT_EQUAL(5, testList.size);
}

// 테스트 8: 역순으로 삽입 (정렬되어야 함)
void test_insert_reverse_order(void)
{
    insertSortedLL(&testList, 9);
    insertSortedLL(&testList, 7);
    insertSortedLL(&testList, 5);
    insertSortedLL(&testList, 3);
    insertSortedLL(&testList, 2);

    // 리스트가 오름차순으로 정렬되어 있는지 확인
    TEST_ASSERT_EQUAL(2, getItemAt(&testList, 0));
    TEST_ASSERT_EQUAL(3, getItemAt(&testList, 1));
    TEST_ASSERT_EQUAL(5, getItemAt(&testList, 2));
    TEST_ASSERT_EQUAL(7, getItemAt(&testList, 3));
    TEST_ASSERT_EQUAL(9, getItemAt(&testList, 4));
}

// 테스트 9: NULL 포인터 체크
void test_null_pointer(void)
{
    int result = insertSortedLL(NULL, 5);
    TEST_ASSERT_EQUAL(-1, result);
}

// 테스트 10: 음수 값 삽입
void test_insert_negative_values(void)
{
    insertSortedLL(&testList, 5);
    insertSortedLL(&testList, 10);

    int result = insertSortedLL(&testList, -3);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(-3, testList.head->item);
    TEST_ASSERT_EQUAL(3, testList.size);
}

// ==================== 헬퍼 함수 구현 ====================

// ListNode *findNode(LinkedList *ll, int index)
// {
//     ListNode *temp;
//     if (ll == NULL || index < 0 || index >= ll->size)
//         return NULL;
//     temp = ll->head;
//     if (temp == NULL || index < 0)
//         return NULL;
//     while (index > 0)
//     {
//         temp = temp->next;
//         if (temp == NULL)
//             return NULL;
//         index--;
//     }
//     return temp;
// }

// int insertNode(LinkedList *ll, int index, int value)
// {
//     ListNode *pre, *cur;
//     if (ll == NULL || index < 0 || index > ll->size + 1)
//         return -1;
//     if (ll->head == NULL || index == 0)
//     {
//         cur = ll->head;
//         ll->head = malloc(sizeof(ListNode));
//         ll->head->item = value;
//         ll->head->next = cur;
//         ll->size++;
//         return 0;
//     }
//     if ((pre = findNode(ll, index - 1)) != NULL)
//     {
//         cur = pre->next;
//         pre->next = malloc(sizeof(ListNode));
//         pre->next->item = value;
//         pre->next->next = cur;
//         ll->size++;
//         return 0;
//     }
//     return -1;
// }

// void removeAllItems(LinkedList *ll)
// {
//     ListNode *cur = ll->head;
//     ListNode *tmp;
//     while (cur != NULL)
//     {
//         tmp = cur->next;
//         free(cur);
//         cur = tmp;
//     }
//     ll->head = NULL;
//     ll->size = 0;
// }

// // ==================== insertSortedLL 구현 (여기에 구현하세요) ====================

// int insertSortedLL(LinkedList *ll, int item)
// {
//     // TODO: 여기에 코드를 작성하세요
//     return -1;
// }

// ==================== Main 함수 ====================

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_insert_into_empty_list);
    RUN_TEST(test_insert_at_end);
    RUN_TEST(test_insert_in_middle);
    RUN_TEST(test_insert_at_beginning);
    RUN_TEST(test_insert_duplicate_value);
    RUN_TEST(test_multiple_duplicates);
    RUN_TEST(test_insert_sequential_values);
    RUN_TEST(test_insert_reverse_order);
    RUN_TEST(test_null_pointer);
    RUN_TEST(test_insert_negative_values);

    return UNITY_END();
}
