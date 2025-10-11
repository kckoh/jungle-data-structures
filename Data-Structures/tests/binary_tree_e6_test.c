#include "unity.h"
#include "binary_tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// --- stdout 캡처용 버퍼 설정 ---
static char output_buffer[256];
static FILE *saved_stdout;
static FILE *temp_file;

void capture_start(void) {
    saved_stdout = stdout;
    temp_file = tmpfile();
    stdout = temp_file;
}

char* capture_end(void) {
    fflush(stdout);
    fseek(temp_file, 0, SEEK_SET);
    size_t len = fread(output_buffer, 1, sizeof(output_buffer) - 1, temp_file);
    output_buffer[len] = '\0';
    fclose(temp_file);
    stdout = saved_stdout;
    return output_buffer;
}

// --- 테스트 공용 루트 ---
static BTNode *root = NULL;

void setUp(void)    { root = NULL; }
void tearDown(void) { removeAll(&root); }

// -----------------------------
// 1️⃣ 빈 트리 → 아무것도 출력되지 않아야 함
void test_empty_tree_no_output(void) {
    root = NULL;
    capture_start();
    printSmallerValues(root, 100);
    char *out = capture_end();
    TEST_ASSERT_EQUAL_STRING("", out);
}

// -----------------------------
// 2️⃣ 단일 노드(작은 값) → 출력되어야 함
void test_single_node_smaller_than_m(void) {
    root = createBTNode(10);
    capture_start();
    printSmallerValues(root, 50);
    char *out = capture_end();
    TEST_ASSERT_EQUAL_STRING("10 ", out);
}

// -----------------------------
// 3️⃣ 단일 노드(큰 값) → 출력되지 않아야 함
void test_single_node_greater_than_m(void) {
    root = createBTNode(80);
    capture_start();
    printSmallerValues(root, 50);
    char *out = capture_end();
    TEST_ASSERT_EQUAL_STRING("", out);
}

// -----------------------------
// 4️⃣ 문제 예시 트리
//              50
//            /    \
//          30      60
//         / \     /  \
//       25  65   10  75
// m=55 → 출력: 50 30 25 10
void test_prompt_example_tree(void) {
    root = createBTNode(50);
    root->left = createBTNode(30);
    root->right = createBTNode(60);
    root->left->left = createBTNode(25);
    root->left->right = createBTNode(65);
    root->right->left = createBTNode(10);
    root->right->right = createBTNode(75);

    capture_start();
    printSmallerValues(root, 55);
    char *out = capture_end();

    // 순회 순서는 in-order 기준: 왼쪽→노드→오른쪽
    // → 출력 결과는 "25 30 50 10 "이 아니라 in-order 상의 작은 값 순서로 나옴
    // 즉 "25 30 10 50 " 또는 "50 30 25 10 "은 구현 방식에 따라 다름
    // 여기서는 간단히 “출력에 해당 값들이 모두 포함되는지”를 확인
    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "50"));
    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "30"));
    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "25"));
    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "10"));
}

// -----------------------------
// 5️⃣ 혼합 트리 m=20 → 5, 10, 15 출력
//         20
//       10    25
//      5 15
void test_mixed_tree_m20(void) {
    root = createBTNode(20);
    root->left = createBTNode(10);
    root->right = createBTNode(25);
    root->left->left = createBTNode(5);
    root->left->right = createBTNode(15);

    capture_start();
    printSmallerValues(root, 20);
    char *out = capture_end();

    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "5"));
    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "10"));
    TEST_ASSERT_NOT_EQUAL(NULL, strstr(out, "15"));
    TEST_ASSERT_EQUAL(NULL, strstr(out, "20"));
    TEST_ASSERT_EQUAL(NULL, strstr(out, "25"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_empty_tree_no_output);
    RUN_TEST(test_single_node_smaller_than_m);
    RUN_TEST(test_single_node_greater_than_m);
    RUN_TEST(test_prompt_example_tree);
    RUN_TEST(test_mixed_tree_m20);
    return UNITY_END();
}
