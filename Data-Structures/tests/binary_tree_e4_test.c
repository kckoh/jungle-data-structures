#include "unity.h"
#include "binary_tree.h"   // BTNode, createBTNode, removeAll, sumOfOddNodes
#include <stdlib.h>

static BTNode *root = NULL;

void setUp(void)    { root = NULL; }
void tearDown(void) { removeAll(&root); }

// 1) 빈 트리 → 0
void test_empty_tree_returns_0(void) {
    root = NULL;
    TEST_ASSERT_EQUAL_INT(0, sumOfOddNodes(root));
}

// 2) 단일 노드(짝수) → 0
void test_single_even_returns_0(void) {
    root = createBTNode(100);
    TEST_ASSERT_EQUAL_INT(0, sumOfOddNodes(root));
}

// 3) 단일 노드(홀수) → 자기 자신
void test_single_odd_returns_value(void) {
    root = createBTNode(7);
    TEST_ASSERT_EQUAL_INT(7, sumOfOddNodes(root));
}

// 4) 문제 예시 트리 (정답 131)
//            50
//         40     60
//       11  35  80  85
// 홀수: 11 + 35 + 85 = 131
void test_sample_tree_returns_131(void) {
    root = createBTNode(50);
    root->left = createBTNode(40);
    root->right = createBTNode(60);
    root->left->left = createBTNode(11);
    root->left->right = createBTNode(35);
    root->right->left = createBTNode(80);
    root->right->right = createBTNode(85);

    TEST_ASSERT_EQUAL_INT(131, sumOfOddNodes(root));
}

// 5) 섞인 트리 (수동 합 검증)
//         10
//       5     22
//     3   8     7
// 홀수: 5 + 3 + 7 = 15
void test_mixed_tree_returns_15(void) {
    root = createBTNode(10);
    root->left = createBTNode(5);
    root->right = createBTNode(22);
    root->left->left = createBTNode(3);
    root->left->right = createBTNode(8);
    root->right->right = createBTNode(7);

    TEST_ASSERT_EQUAL_INT(15, sumOfOddNodes(root));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_empty_tree_returns_0);
    RUN_TEST(test_single_even_returns_0);
    RUN_TEST(test_single_odd_returns_value);
    RUN_TEST(test_sample_tree_returns_131);
    RUN_TEST(test_mixed_tree_returns_15);
    return UNITY_END();
}
