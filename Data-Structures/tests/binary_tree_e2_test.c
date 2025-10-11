#include "unity.h"
#include "binary_tree.h"   // BTNode, createBTNode, removeAll, maxHeight
#include <stdlib.h>

static BTNode *root = NULL;

void setUp(void)    { root = NULL; }
void tearDown(void) { removeAll(&root); }

// 빈 트리: 높이 = -1
void test_empty_tree(void) {
    root = NULL;
    TEST_ASSERT_EQUAL_INT(-1, maxHeight(root));
}

// 단일 노드: 높이 = 0
void test_single_node_height_0(void) {
    root = createBTNode(42);
    TEST_ASSERT_EQUAL_INT(0, maxHeight(root));
}

// 완전 이진트리 (1..7): 높이 = 2
//      4
//    2   6
//   1 3 5 7
void test_perfect_tree_height_2(void) {
    root = createBTNode(4);
    root->left = createBTNode(2);
    root->right = createBTNode(6);
    root->left->left = createBTNode(1);
    root->left->right = createBTNode(3);
    root->right->left = createBTNode(5);
    root->right->right = createBTNode(7);
    TEST_ASSERT_EQUAL_INT(2, maxHeight(root));
}

// 왼쪽 편향(노드 5개): 높이 = 4
void test_left_skewed_height_4(void) {
    root = createBTNode(1);
    root->left = createBTNode(2);
    root->left->left = createBTNode(3);
    root->left->left->left = createBTNode(4);
    root->left->left->left->left = createBTNode(5);
    TEST_ASSERT_EQUAL_INT(4, maxHeight(root));
}

// 비대칭: 기대 3
//        8
//      4   12
//     2 6
//    1
void test_unbalanced_height_3(void) {
    root = createBTNode(8);
    root->left = createBTNode(4);
    root->right = createBTNode(12);
    root->left->left = createBTNode(2);
    root->left->right = createBTNode(6);
    root->left->left->left = createBTNode(1);
    TEST_ASSERT_EQUAL_INT(3, maxHeight(root));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_empty_tree);
    RUN_TEST(test_single_node_height_0);
    RUN_TEST(test_perfect_tree_height_2);
    RUN_TEST(test_left_skewed_height_4);
    RUN_TEST(test_unbalanced_height_3);
    return UNITY_END();
}
