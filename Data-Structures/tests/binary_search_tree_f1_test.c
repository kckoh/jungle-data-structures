#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // dup, dup2
#include <fcntl.h>    // open

#include "binary_search_tree.h"

// ---- stdout capture helpers ----
static int begin_capture(const char *tmp_path) {
    fflush(stdout);
    int saved = dup(fileno(stdout));              // duplicate current stdout fd
    FILE *f = freopen(tmp_path, "w+", stdout);    // redirect stdout to file
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "Failed to redirect stdout");
    return saved;
}

static void end_capture(int saved_fd) {
    fflush(stdout);
    dup2(saved_fd, fileno(stdout));               // restore stdout
    close(saved_fd);
}

// Read integers (tokens of [-]?[0-9]+) from file into out[], return count.
// Ignores commas, spaces, newlines, etc. Robust to "20, 15 50\n" style.
static int read_ints_from_file(const char *path, int *out, int maxn) {
    FILE *f = fopen(path, "r");
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "Failed to open capture file");
    int c, sign = 1, have = 0, in_num = 0, val = 0, cnt = 0;
    while ((c = fgetc(f)) != EOF) {
        if (c == '-') {
            if (in_num) { // treat as separator if already in number
                if (cnt < maxn) out[cnt++] = sign * val;
                val = 0; sign = 1; in_num = 0;
            }
            sign = -1;
            in_num = 1;
            val = 0;
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

// ---- Fixtures ----
static BSTNode *root;

void setUp(void) {
    root = NULL;
}

void tearDown(void) {
    removeAll(&root);
}

// Helper to bulk insert into BST
static void insert_many(BSTNode **r, const int *vals, int n) {
    for (int i = 0; i < n; i++) insertBSTNode(r, vals[i]);
}

// Assert traversal equals expected sequence (order only; formatting ignored)
static void assert_level_order_equals(const int *expected, int n_expected) {
    const char *CAP = "test_levelorder_capture.txt";
    int saved = begin_capture(CAP);

    levelOrderTraversal(root);

    end_capture(saved);

    int actual[1024];
    int n_actual = read_ints_from_file(CAP, actual, 1024);

    TEST_ASSERT_EQUAL_INT_MESSAGE(
        n_expected, n_actual, "Number of integers printed differs from expected");

    for (int i = 0; i < n_expected; i++) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Mismatch at index %d", i);
        TEST_ASSERT_EQUAL_INT_MESSAGE(expected[i], actual[i], msg);
    }

    // optional: cleanup file
    remove(CAP);
}

// -------------------- TESTS --------------------

// Empty tree → prints nothing (no integers)
void test_empty_tree_prints_nothing(void) {
    const int expected[] = {}; // none
    assert_level_order_equals(expected, 0);
}

// Single node → prints that single value
void test_single_node_only_root(void) {
    int vals[] = {42};
    insert_many(&root, vals, 1);
    const int expected[] = {42};
    assert_level_order_equals(expected, 1);
}

// Sample from prompt: 20, 15, 50, 10, 18, 25, 80
// Build via BST inserts that produce the shown structure.
void test_prompt_example_level_order(void) {
    int vals[] = {20, 15, 50, 10, 18, 25, 80};
    insert_many(&root, vals, sizeof(vals)/sizeof(vals[0]));

    const int expected[] = {20, 15, 50, 10, 18, 25, 80};
    assert_level_order_equals(expected, sizeof(expected)/sizeof(expected[0]));
}

// A deeper tree to catch queue correctness across multiple levels
void test_deeper_tree_multiple_levels(void) {
    // This set creates multiple levels and interleaving left/right
    int vals[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65, 85};
    insert_many(&root, vals, sizeof(vals)/sizeof(vals[0]));
    // Expected level order:
    // L0: 50
    // L1: 30, 70
    // L2: 20, 40, 60, 80
    // L3: 35, 45, 65, 85
    const int expected[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65, 85};
    assert_level_order_equals(expected, sizeof(expected)/sizeof(expected[0]));
}

int main(void) {
    UNITY_BEGIN();
    // RUN_TEST(test_empty_tree_prints_nothing);
    // RUN_TEST(test_single_node_only_root);
    RUN_TEST(test_prompt_example_level_order);
    RUN_TEST(test_deeper_tree_multiple_levels);
    return UNITY_END();
}
