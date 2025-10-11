// tests/binary_tree_identical_test.c
#include "unity.h"
#include <stdlib.h>
#include "binary_tree.h"  // BTNode, createBTNode, removeAll, identical

/* 전역 포인터: 각 TC에서 만들고 tearDown에서 해제 */
static BTNode *t1, *t2;

/* --- 헬퍼: 안전 연결 --- */
static BTNode* n(int v) { return createBTNode(v); }

/* 구조:
       5
     /   \
    3     7
   / \   / \
  1  2  4   8
*/
static BTNode* make_example_tree(void) {
    BTNode *root = n(5);
    root->left = n(3);
    root->right = n(7);
    root->left->left = n(1);
    root->left->right = n(2);
    root->right->left = n(4);
    root->right->right = n(8);
    return root;
}

/* Unity hooks */
void setUp(void)   { t1 = NULL; t2 = NULL; }
void tearDown(void){ removeAll(&t1); removeAll(&t2); }

/* --- Test Cases --- */

/* 0) 둘 다 NULL */
void test_identical_both_null(void) {
    TEST_ASSERT_EQUAL_INT(1, identical(NULL, NULL));
}

/* 1) 한쪽만 NULL */
void test_identical_one_null(void) {
    t1 = n(42);
    TEST_ASSERT_EQUAL_INT(0, identical(t1, NULL));
    TEST_ASSERT_EQUAL_INT(0, identical(NULL, t1));
}

/* 2) 단일 노드 동일 값 */
void test_identical_single_node_same_value(void) {
    t1 = n(7);
    t2 = n(7);
    TEST_ASSERT_EQUAL_INT(1, identical(t1, t2));
}

/* 3) 단일 노드 다른 값 */
void test_identical_single_node_diff_value(void) {
    t1 = n(7);
    t2 = n(9);
    TEST_ASSERT_EQUAL_INT(0, identical(t1, t2));
}

/* 4) 예시 트리: 구조/값 모두 동일 */
void test_identical_example_same_shape_and_values(void) {
    t1 = make_example_tree();
    t2 = make_example_tree();
    TEST_ASSERT_EQUAL_INT(1, identical(t1, t2));
}

/* 5) 구조는 동일하나, 값 하나만 다름 */
void test_identical_same_shape_diff_value(void) {
    t1 = make_example_tree();
    t2 = make_example_tree();
    t2->left->right->item = 999;  // 원래 2였던 자리 값을 바꿈
    TEST_ASSERT_EQUAL_INT(0, identical(t1, t2));
}

/* 6) 값 멀티셋은 같지만 구조가 다르면 실패 */
void test_identical_diff_shape_same_values_multiset(void) {
    // t1: 예시 트리(완전 이진)
    t1 = make_example_tree();

    // t2: 2를 1의 오른쪽 자식으로 옮겨 구조 변경 (값 집합은 동일)
    t2 = n(5);
    t2->left = n(3);
    t2->right = n(7);
    t2->left->left = n(1);
    t2->left->left->right = n(2);   // <— 구조 차이
    t2->right->left = n(4);
    t2->right->right = n(8);

    TEST_ASSERT_EQUAL_INT(0, identical(t1, t2));
}

/* 7) 스키니(오른쪽 사슬) vs. 더 균형 잡힌 트리 */
void test_identical_skinny_vs_balanced(void) {
    // t1: 1 -> 2 -> 3 -> 4 (오른쪽으로만 연속)
    t1 = n(1);
    t1->right = n(2);
    t1->right->right = n(3);
    t1->right->right->right = n(4);

    // t2: 동일 값이지만 다른 모양
    t2 = n(2);
    t2->left = n(1);
    t2->right = n(3);
    t2->right->right = n(4);

    TEST_ASSERT_EQUAL_INT(0, identical(t1, t2));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_identical_both_null);
    RUN_TEST(test_identical_one_null);
    RUN_TEST(test_identical_single_node_same_value);
    RUN_TEST(test_identical_single_node_diff_value);
    RUN_TEST(test_identical_example_same_shape_and_values);
    RUN_TEST(test_identical_same_shape_diff_value);
    RUN_TEST(test_identical_diff_shape_same_values_multiset);
    RUN_TEST(test_identical_skinny_vs_balanced);
    return UNITY_END();
}
