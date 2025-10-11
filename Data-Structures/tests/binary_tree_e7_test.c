#include "unity.h"
#include "binary_tree.h"   // BTNode, createBTNode, removeAll, smallestValue
#include <stdlib.h>

// 공용 루트
static BTNode *root = NULL;

void setUp(void)    { root = NULL; }
void tearDown(void) { removeAll(&root); }

/* 주의:
 * smallestValue(NULL)의 반환값(정의)은 과제 명세에 없으므로
 * 본 테스트에서는 "비어있지 않은 트리"만 검증합니다.
 */

// 1) 단일 노드 → 그 노드 값
void test_single_node_returns_self(void) {
    root = createBTNode(42);
    TEST_ASSERT_EQUAL_INT(42, smallestValue(root));
}

// 2) 문제 예시 트리 (정답 10)
//              50
//            /    \
//          30      60
//         / \     /  \
//       25  65   10  75
void test_prompt_example_returns_10(void) {
    root = createBTNode(50);
    root->left = createBTNode(30);
    root->right = createBTNode(60);
    root->left->left = createBTNode(25);
    root->left->right = createBTNode(65);
    root->right->left = createBTNode(10);
    root->right->right = createBTNode(75);

    TEST_ASSERT_EQUAL_INT(10, smallestValue(root));
}

// 3) 왼쪽 편향 트리 → 맨 마지막 값이 최소
//  9 -> 7 -> 5 -> 3 -> 1
void test_left_skewed_returns_last_left(void) {
    root = createBTNode(9);
    root->left = createBTNode(7);
    root->left->left = createBTNode(5);
    root->left->left->left = createBTNode(3);
    root->left->left->left->left = createBTNode(1);

    TEST_ASSERT_EQUAL_INT(1, smallestValue(root));
}

// 4) 오른쪽 편향 트리(정렬 X, 일반 이진트리) → 중간에 더 작은 값이 있어야 함
//    20
//      \
//       50
//      /
//     4
//    / \
//   9   2   -> 최소는 2
void test_right_skewed_with_small_on_left_subtree(void) {
    root = createBTNode(20);
    root->right = createBTNode(50);
    root->right->left = createBTNode(4);
    root->right->left->left = createBTNode(9);
    root->right->left->right = createBTNode(2);

    TEST_ASSERT_EQUAL_INT(2, smallestValue(root));
}

// 5) 값이 섞인 비대칭 트리
//        8
//      14   3
//         7   11
// 최소: 3
void test_unbalanced_mixed_returns_3(void) {
    root = createBTNode(8);
    root->left = createBTNode(14);
    root->right = createBTNode(3);
    root->right->left = createBTNode(7);
    root->right->right = createBTNode(11);

    TEST_ASSERT_EQUAL_INT(3, smallestValue(root));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_single_node_returns_self);
    RUN_TEST(test_prompt_example_returns_10);
    RUN_TEST(test_left_skewed_returns_last_left);
    RUN_TEST(test_right_skewed_with_small_on_left_subtree);
    RUN_TEST(test_unbalanced_mixed_returns_3);
    return UNITY_END();
}
