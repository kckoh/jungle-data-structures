// tests/binary_search_tree_inorder_iterative_test.c
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // dup, dup2
#include <fcntl.h>

#include "binary_search_tree.h" // 여러분의 헤더 파일명에 맞게 조정

// ---------- stdout capture helpers ----------
static int begin_capture(const char *tmp_path) {
    fflush(stdout);
    int saved = dup(fileno(stdout));
    FILE *f = freopen(tmp_path, "w+", stdout);
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "Failed to redirect stdout");
    return saved;
}

static void end_capture(int saved_fd) {
    fflush(stdout);
    dup2(saved_fd, fileno(stdout));
    close(saved_fd);
}

// 콤마/공백/개행 등 구분자에 둔감하게 정수만 파싱
static int read_ints_from_file(const char *path, int *out, int maxn) {
    FILE *f = fopen(path, "r");
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "Failed to open capture file");
    int c, sign = 1, in_num = 0, val = 0, cnt = 0;
    while ((c = fgetc(f)) != EOF) {
        if (c == '-') {
            if (in_num) {
                if (cnt < maxn) out[cnt++] = sign * val;
                val = 0; sign = 1; in_num = 0;
            }
            sign = -1; in_num = 1; val = 0;
        } else if (c >= '0' && c <= '9') {
            if (!in_num) { in_num = 1; sign = 1; val = 0; }
            val = val * 10 + (c - '0');
        } else {
            if (in_num) {
                if (cnt < maxn) out[cnt++] = sign * val;
                val = 0; sign = 1; in_num = 0;
            }
        }
    }
    if (in_num && cnt < maxn) out[cnt++] = sign * val;
    fclose(f);
    return cnt;
}

// ---------- Fixtures ----------
static BSTNode *root;

void setUp(void)   { root = NULL; }
void tearDown(void){ removeAll(&root); }

static void insert_many(BSTNode **r, const int *vals, int n) {
    for (int i = 0; i < n; i++) insertBSTNode(r, vals[i]);
}

static void assert_inorder_equals(const int *expected, int n_expected) {
    const char *CAP = "capture_inorder_iter.txt";
    int saved = begin_capture(CAP);

    inOrderIterative(root);

    end_capture(saved);

    int actual[1024];
    int n_actual = read_ints_from_file(CAP, actual, 1024);

    TEST_ASSERT_EQUAL_INT_MESSAGE(
        n_expected, n_actual, "Printed integer count differs from expected");

    for (int i = 0; i < n_expected; i++) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Mismatch at index %d", i);
        TEST_ASSERT_EQUAL_INT_MESSAGE(expected[i], actual[i], msg);
    }
    remove(CAP);
}

// ---------- Tests ----------

// 1) 빈 트리: 출력 없음
void test_empty_tree_prints_nothing(void) {
    const int expected[] = {};
    assert_inorder_equals(expected, 0);
}

// 2) 루트만 있는 트리
void test_single_node(void) {
    int vals[] = {42};
    insert_many(&root, vals, 1);
    const int expected[] = {42};
    assert_inorder_equals(expected, 1);
}

// 3) 문제 예시 구조: {20, 15, 50, 10, 18} → Inorder: 10, 15, 18, 20, 50
void test_prompt_example_inorder(void) {
    int vals[] = {20, 15, 50, 10, 18};
    insert_many(&root, vals, sizeof(vals)/sizeof(vals[0]));
    const int expected[] = {10, 15, 18, 20, 50};
    assert_inorder_equals(expected, sizeof(expected)/sizeof(expected[0]));
}

// 4) 우측 편향(증가 수열 삽입) → 오름차순 그대로
void test_right_skewed_tree(void) {
    int vals[] = {10, 20, 30, 40, 50};
    insert_many(&root, vals, 5);
    const int expected[] = {10, 20, 30, 40, 50};
    assert_inorder_equals(expected, 5);
}

// 5) 다층 일반 트리
void test_deeper_tree_inorder(void) {
    int vals[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65, 85};
    insert_many(&root, vals, sizeof(vals)/sizeof(vals[0]));
    const int expected[] = {20, 30, 35, 40, 45, 50, 60, 65, 70, 80, 85};
    assert_inorder_equals(expected, sizeof(expected)/sizeof(expected[0]));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_empty_tree_prints_nothing);
    RUN_TEST(test_single_node);
    RUN_TEST(test_prompt_example_inorder);
    RUN_TEST(test_right_skewed_tree);
    RUN_TEST(test_deeper_tree_inorder);
    return UNITY_END();
}
