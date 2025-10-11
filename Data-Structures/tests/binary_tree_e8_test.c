#include "unity.h"
#include "binary_tree.h"   // BTNode, createBTNode, removeAll, hasGreatGrandchild
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* -------- stdout 캡처 유틸 -------- */
static char output_buffer[512];
static FILE *saved_stdout;
static FILE *temp_file;

static void capture_start(void) {
    saved_stdout = stdout;
    temp_file = tmpfile();
    stdout = temp_file;
}

static const char* capture_end(void) {
    fflush(stdout);
    fseek(temp_file, 0, SEEK_SET);
    size_t len = fread(output_buffer, 1, sizeof(output_buffer) - 1, temp_file);
    output_buffer[len] = '\0';
    fclose(temp_file);
    stdout = saved_stdout;
    return output_buffer;
}

/* -------- 공용 루트 -------- */
static BTNode *root = NULL;

void setUp(void)    { root = NULL; }
void tearDown(void) { removeAll(&root); }

/* ===================== 테스트 케이스 ===================== */

/* 1) 빈 트리: 출력 없음, 반환 0 */
void test_empty_tree_prints_nothing_and_returns_0(void) {
    root = NULL;
    capture_start();
    int count = hasGreatGrandchild(root);
    const char *out = capture_end();
    TEST_ASSERT_EQUAL_INT(0, count);
    TEST_ASSERT_EQUAL_STRING("", out);
}

/* 2) 단일 노드: 출력 없음, 반환 0 */
void test_single_node_returns_0(void) {
    root = createBTNode(42);
    capture_start();
    int count = hasGreatGrandchild(root);
    const char *out = capture_end();
    TEST_ASSERT_EQUAL_INT(0, count);
    TEST_ASSERT_EQUAL_STRING("", out);
}

/* 3) 깊이 5의 선형 트리 (왼쪽 편향):
 *   1 - 2 - 3 - 4 - 5
 * 깊이(간선) 기준: 루트(1)는 3 간선 아래(4)가 있으므로 해당,
 *                  2도 3 간선 아래(5)가 있으므로 해당.
 * 기대: 2개 노드가 출력되고 반환 2.
 * (출력 순서는 구현에 따라 다를 수 있으니 포함 여부만 확인)
 */
void test_left_skewed_depth5_has_two_nodes(void) {
    root = createBTNode(1);
    root->left = createBTNode(2);
    root->left->left = createBTNode(3);
    root->left->left->left = createBTNode(4);
    root->left->left->left->left = createBTNode(5);

    capture_start();
    int count = hasGreatGrandchild(root);
    const char *out = capture_end();

    TEST_ASSERT_EQUAL_INT(2, count);
    /* 1 또는 2가 출력되도록 구현될 수 있으나,
       '조건을 만족하는 노드'는 1과 2 두 개여야 하므로 둘 다 포함을 기대 */
    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "1"));
    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "2"));
}

/* 4) 문제 예시 트리: 결과는 50 하나만 출력, 반환 1
 *            50
 *          /     \
 *        30       60
 *       /  \     /  \
 *     25   65   10   75
 *          /           \
 *         20            15
 * 경로: 50->60->75->15 (간선 3) → 50만 해당
 */
void test_prompt_example_prints_50_only(void) {
    root = createBTNode(50);
    root->left = createBTNode(30);
    root->right = createBTNode(60);
    root->left->left = createBTNode(25);
    root->left->right = createBTNode(65);
    root->left->right->left = createBTNode(20);
    root->right->left = createBTNode(10);
    root->right->right = createBTNode(75);
    root->right->right->right = createBTNode(15);

    capture_start();
    int count = hasGreatGrandchild(root);
    const char *out = capture_end();

    TEST_ASSERT_EQUAL_INT(1, count);
    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "50"));

    /* 다른 후보들이 출력되지 않았는지 간단 체크(엄격 필수는 아님) */
    TEST_ASSERT_EQUAL(NULL, strstr(out, "30"));
    TEST_ASSERT_EQUAL(NULL, strstr(out, "60"));
    TEST_ASSERT_EQUAL(NULL, strstr(out, "65"));
    TEST_ASSERT_EQUAL(NULL, strstr(out, "75"));
}

/* 5) 높이 2(간선) 완전 이진트리: 어떤 노드도 증손(3간선 아래) 없음 → 0
 *        4
 *      2   6
 *     1 3 5 7
 */
void test_perfect_height2_returns_0(void) {
    root = createBTNode(4);
    root->left = createBTNode(2);
    root->right = createBTNode(6);
    root->left->left = createBTNode(1);
    root->left->right = createBTNode(3);
    root->right->left = createBTNode(5);
    root->right->right = createBTNode(7);

    capture_start();
    int count = hasGreatGrandchild(root);
    const char *out = capture_end();

    TEST_ASSERT_EQUAL_INT(0, count);
    TEST_ASSERT_EQUAL_STRING("", out);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_empty_tree_prints_nothing_and_returns_0);
    RUN_TEST(test_single_node_returns_0);
    RUN_TEST(test_left_skewed_depth5_has_two_nodes);
    RUN_TEST(test_prompt_example_prints_50_only);
    RUN_TEST(test_perfect_height2_returns_0);
    return UNITY_END();
}
