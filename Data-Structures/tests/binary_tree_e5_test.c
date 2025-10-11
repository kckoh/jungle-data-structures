#include "unity.h"
#include "binary_tree.h"   // BTNode, createBTNode, removeAll, mirrorTree, identical
#include <stdlib.h>

static BTNode *root = NULL;
static BTNode *expected = NULL;

void setUp(void) {
    root = NULL;
    expected = NULL;
}

void tearDown(void) {
    removeAll(&root);
    removeAll(&expected);
}

/* ========== 헬퍼: 트리 빌더들 ========== */

// 깊이 1:      1
static BTNode* build_single(void) {
    return createBTNode(1);
}

// 깊이 2:       2
//             /   \
//            1     3
static BTNode* build_simple_balanced(void) {
    BTNode *r = createBTNode(2);
    r->left = createBTNode(1);
    r->right = createBTNode(3);
    return r;
}

// 깊이 2 미러:   2
//              /   \
//             3     1
static BTNode* build_simple_balanced_mirror(void) {
    BTNode *r = createBTNode(2);
    r->left = createBTNode(3);
    r->right = createBTNode(1);
    return r;
}

// 문제 서술 예시 트리 (중위 출력: 5 6 4 3 2 1)
// 원본 구조(값 그대로 사용):
//              4
//            /   \
//           5     2
//            \   / \
//             6 3   1
static BTNode* build_prompt_example_original(void) {
    BTNode *r = createBTNode(4);
    r->left = createBTNode(5);
    r->right = createBTNode(2);

    r->left->right = createBTNode(6);

    r->right->left = createBTNode(3);
    r->right->right = createBTNode(1);
    return r;
}

// 위 트리의 미러(중위 출력: 1 2 3 4 6 5)
//              4
//            /   \
//           2     5
//          / \     \
//         1   3     6
static BTNode* build_prompt_example_mirror(void) {
    BTNode *r = createBTNode(4);
    r->left = createBTNode(2);
    r->right = createBTNode(5);

    r->left->left = createBTNode(1);
    r->left->right = createBTNode(3);

    // 수정: 오른쪽(5)의 자식 6은 '왼쪽'에 붙습니다.
    r->right->left = createBTNode(6);
    return r;
}

/* ========== 테스트 케이스 ========== */

// 1) NULL 입력: 크래시 없이 동작, 변화 없음
void test_mirror_null_tree_ok(void) {
    root = NULL;
    mirrorTree(root);  // should not crash
    TEST_ASSERT_EQUAL_INT(1, identical(root, NULL)); // 둘 다 NULL → identical = 1
}

// 2) 단일 노드: 미러해도 동일
void test_mirror_single_no_change(void) {
    root = build_single();
    expected = build_single();
    mirrorTree(root);
    TEST_ASSERT_EQUAL_INT(1, identical(root, expected));
}

// 3) 간단 완전 이진트리: 좌우가 뒤바뀌어야 함
void test_mirror_simple_balanced(void) {
    root = build_simple_balanced();             // 2 / {1,3}
    expected = build_simple_balanced_mirror();  // 2 / {3,1}
    mirrorTree(root);
    TEST_ASSERT_EQUAL_INT(1, identical(root, expected));
}

// 4) 문제 예시 트리: 미러 결과가 기대 트리와 동일해야 함
void test_mirror_prompt_example_tree(void) {
    root = build_prompt_example_original();
    expected = build_prompt_example_mirror();
    mirrorTree(root);
    TEST_ASSERT_EQUAL_INT(1, identical(root, expected));
}

// 5) 미러를 두 번 하면 원래 트리로 돌아와야 함
void test_double_mirror_returns_to_original(void) {
    // 원본과 비교용 원본 복제(동일 구조로 새로 생성)
    root = build_prompt_example_original();
    expected = build_prompt_example_original();

    mirrorTree(root);
    mirrorTree(root);   // 두 번 미러

    TEST_ASSERT_EQUAL_INT(1, identical(root, expected));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_mirror_null_tree_ok);
    RUN_TEST(test_mirror_single_no_change);
    RUN_TEST(test_mirror_simple_balanced);
    RUN_TEST(test_mirror_prompt_example_tree);
    RUN_TEST(test_double_mirror_returns_to_original);
    return UNITY_END();
}
