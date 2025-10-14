#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary_search_tree.h"

// ---------- Helpers ----------
static BSTNode *root;

void setUp(void)   { root = NULL; }
void tearDown(void){ removeAll(&root); }

static void insert_many(BSTNode **r, const int *vals, int n) {
    for (int i = 0; i < n; i++) insertBSTNode(r, vals[i]);
}

static void capture_postorder(BSTNode *r, int *out, int *count) {
    // 간단히 stdout 캡처 안 하고 함수 직접 구현: recursion 기반
    if (!r) return;
    capture_postorder(r->left, out, count);
    capture_postorder(r->right, out, count);
    out[(*count)++] = r->item;
}

static void assert_postorder_equals(const int *expected, int n_expected) {
    int actual[64];
    int n_actual = 0;
    capture_postorder(root, actual, &n_actual);

    TEST_ASSERT_EQUAL_INT_MESSAGE(n_expected, n_actual, "Node count mismatch");
    for (int i = 0; i < n_expected; i++) {
        char msg[64];
        snprintf(msg, sizeof(msg), "Mismatch at index %d", i);
        TEST_ASSERT_EQUAL_INT_MESSAGE(expected[i], actual[i], msg);
    }
}

// ---------- Tests ----------

// 1️⃣ 빈 트리에서 삭제
void test_delete_from_empty_tree(void) {
    root = removeNodeFromTree(root, 10);
    TEST_ASSERT_NULL(root);
}

// 2️⃣ 리프 노드 삭제
void test_delete_leaf_node(void) {
    int vals[] = {20, 10, 30};
    insert_many(&root, vals, 3);
    root = removeNodeFromTree(root, 10);

    // 후위순회: 왼쪽(없음), 오른쪽(30), 루트(20)
    const int expected[] = {30, 20};
    assert_postorder_equals(expected, 2);
}

// 3️⃣ 한쪽 자식만 있는 노드 삭제
void test_delete_node_with_one_child(void) {
    int vals[] = {50, 30, 70, 20};
    insert_many(&root, vals, 4);
    root = removeNodeFromTree(root, 30);

    // 30의 자식(20)이 위로 올라감
    const int expected[] = {20, 70, 50};
    assert_postorder_equals(expected, 3);
}

// 4️⃣ 두 자식이 있는 노드 삭제 (후임자 방식)
void test_delete_node_with_two_children(void) {
    int vals[] = {50, 30, 70, 20, 40, 60, 80};
    insert_many(&root, vals, 7);

    root = removeNodeFromTree(root, 50); // 루트 삭제
    // 후임자 60이 루트로 올라옴
    const int expected[] = {20, 40, 30, 80, 70, 60};
    assert_postorder_equals(expected, 6);
}

// 5️⃣ 여러 번 삭제 연속 테스트
void test_multiple_deletes(void) {
    int vals[] = {40, 20, 60, 10, 30, 50, 70};
    insert_many(&root, vals, 7);

    root = removeNodeFromTree(root, 10);
    root = removeNodeFromTree(root, 60);
    root = removeNodeFromTree(root, 40);

    const int expected[] = {30, 20, 70, 50};
    assert_postorder_equals(expected, 4);
}

// ---------- Entry ----------
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_delete_from_empty_tree);
    RUN_TEST(test_delete_leaf_node);
    RUN_TEST(test_delete_node_with_one_child);
    RUN_TEST(test_delete_node_with_two_children);
    RUN_TEST(test_multiple_deletes);
    return UNITY_END();
}
