#include "unity.h"
#include "binary_tree.h"   // BTNode, createBTNode, removeAll, countOneChildNodes
#include <stdlib.h>

static BTNode *root = NULL;

void setUp(void)    { root = NULL; }
void tearDown(void) { removeAll(&root); }

// 1) 빈 트리 → 0
void test_empty_tree_returns_0(void) {
    root = NULL;
    TEST_ASSERT_EQUAL_INT(0, countOneChildNodes(root));
}

// 2) 단일 노드 → 0
void test_single_node_returns_0(void) {
    root = createBTNode(1);
    TEST_ASSERT_EQUAL_INT(0, countOneChildNodes(root));
}

// 3) 완전 이진트리(1..7) → 0
void test_perfect_tree_returns_0(void) {
    root = createBTNode(4);
    root->left = createBTNode(2);
    root->right = createBTNode(6);
    root->left->left = createBTNode(1);
    root->left->right = createBTNode(3);
    root->right->left = createBTNode(5);
    root->right->right = createBTNode(7);
    TEST_ASSERT_EQUAL_INT(0, countOneChildNodes(root));
}

// 4) 문제 예시 트리: 정답 2
//            50
//         20     60
//       10  30     80
//          55
// 한 자식만: 30(왼쪽만 55), 60(오른쪽만 80)
void test_sample_tree_returns_2(void) {
    root = createBTNode(50);
    root->left = createBTNode(20);
    root->right = createBTNode(60);
    root->left->left = createBTNode(10);
    root->left->right = createBTNode(30);
    root->left->right->left = createBTNode(55);
    root->right->right = createBTNode(80);
    TEST_ASSERT_EQUAL_INT(2, countOneChildNodes(root));
}

// 5) 왼쪽 편향 5노드 → 4
void test_left_skewed_returns_4(void) {
    root = createBTNode(1);
    root->left = createBTNode(2);
    root->left->left = createBTNode(3);
    root->left->left->left = createBTNode(4);
    root->left->left->left->left = createBTNode(5);
    TEST_ASSERT_EQUAL_INT(4, countOneChildNodes(root));
}

// 6) 혼합 비대칭 → 3
//        8
//      4    12
//     2       14
//              15
// 한 자식만: 4, 12, 14
void test_unbalanced_mixed_returns_3(void) {
    root = createBTNode(8);
    root->left = createBTNode(4);
    root->right = createBTNode(12);
    root->left->left = createBTNode(2);
    root->right->right = createBTNode(14);
    root->right->right->right = createBTNode(15);
    TEST_ASSERT_EQUAL_INT(3, countOneChildNodes(root));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_empty_tree_returns_0);
    RUN_TEST(test_single_node_returns_0);
    RUN_TEST(test_perfect_tree_returns_0);
    RUN_TEST(test_sample_tree_returns_2);
    RUN_TEST(test_left_skewed_returns_4);
    RUN_TEST(test_unbalanced_mixed_returns_3);
    return UNITY_END();
}
