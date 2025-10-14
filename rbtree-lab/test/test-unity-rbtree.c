#include "unity.h"
#include "unity_internals.h"
#include <rbtree.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

// new_rbtree should return rbtree struct with null root node
void test_init(void) {
  rbtree *t = new_rbtree();
  TEST_ASSERT_NOT_NULL(t);
#ifdef SENTINEL
  TEST_ASSERT_NOT_NULL(t->nil);
  TEST_ASSERT_EQUAL_PTR(t->root, t->nil);
#else
  TEST_ASSERT_NULL(t->root);
#endif
  delete_rbtree(t);
}

// root node should have proper values and pointers
void test_insert_single(void) {
  key_t key = 1024;
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_PTR(t->root, p);
  TEST_ASSERT_EQUAL_INT(key, p->key);
#ifdef SENTINEL
  TEST_ASSERT_EQUAL_PTR(p->left, t->nil);
  TEST_ASSERT_EQUAL_PTR(p->right, t->nil);
  TEST_ASSERT_EQUAL_PTR(p->parent, t->nil);
#else
  TEST_ASSERT_NULL(p->left);
  TEST_ASSERT_NULL(p->right);
  TEST_ASSERT_NULL(p->parent);
#endif
  delete_rbtree(t);
}

// find should return the node with the key or NULL if no such node exists
void test_find_single(void) {
  key_t key = 512, wrong_key = 1024;
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);

  node_t *q = rbtree_find(t, key);
  TEST_ASSERT_NOT_NULL(q);
  TEST_ASSERT_EQUAL_INT(key, q->key);
  TEST_ASSERT_EQUAL_PTR(p, q);

  q = rbtree_find(t, wrong_key);
  TEST_ASSERT_NULL(q);

  delete_rbtree(t);
}

// erase should delete root node
void test_erase_root(void) {
  key_t key = 128;
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_PTR(t->root, p);
  TEST_ASSERT_EQUAL_INT(key, p->key);

  rbtree_erase(t, p);
#ifdef SENTINEL
  TEST_ASSERT_EQUAL_PTR(t->root, t->nil);
#else
  TEST_ASSERT_NULL(t->root);
#endif

  delete_rbtree(t);
}

/* ------------------------
   Helper functions
------------------------- */

static void insert_arr(rbtree *t, const key_t *arr, const size_t n) {
  for (size_t i = 0; i < n; i++) {
    rbtree_insert(t, arr[i]);
  }
}

static int comp(const void *p1, const void *p2) {
  const key_t *e1 = (const key_t *)p1;
  const key_t *e2 = (const key_t *)p2;
  return (*e1 > *e2) - (*e1 < *e2);
}

/* ------------------------
   Min/Max Tests
------------------------- */

void test_minmax_suite(void) {
  key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  size_t n = sizeof(arr) / sizeof(arr[0]);
  rbtree *t = new_rbtree();

  insert_arr(t, arr, n);
  qsort(arr, n, sizeof(key_t), comp);

  node_t *min = rbtree_min(t);
  TEST_ASSERT_NOT_NULL(min);
  TEST_ASSERT_EQUAL_INT(arr[0], min->key);

  node_t *max = rbtree_max(t);
  TEST_ASSERT_NOT_NULL(max);
  TEST_ASSERT_EQUAL_INT(arr[n - 1], max->key);

  delete_rbtree(t);
}

/* ------------------------
   Array conversion Tests
------------------------- */

void test_to_array_suite(void) {
  key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  rbtree *t = new_rbtree();

  insert_arr(t, entries, n);
  qsort(entries, n, sizeof(key_t), comp);

  key_t *res = calloc(n, sizeof(key_t));
  rbtree_to_array(t, res, n);
  for (size_t i = 0; i < n; i++) {
    TEST_ASSERT_EQUAL_INT(entries[i], res[i]);
  }

  free(res);
  delete_rbtree(t);
}

/* ------------------------
   Example randomized test
------------------------- */

void test_find_erase_rand(void) {
  const size_t n = 1000;
  srand(17);
  rbtree *t = new_rbtree();
  key_t *arr = calloc(n, sizeof(key_t));
  for (size_t i = 0; i < n; i++) arr[i] = rand();

  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    TEST_ASSERT_NOT_NULL(p);
  }
  delete_rbtree(t);
  free(arr);
}

void test_delete_empty_tree(void) {
  rbtree *t = new_rbtree();
  delete_rbtree(t);   // should not crash
  TEST_PASS();        // if it reaches here, it's good
}

/* ------------------------
   Test runner
------------------------- */

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_init);
  RUN_TEST(test_delete_empty_tree);
  RUN_TEST(test_insert_single);
  RUN_TEST(test_find_single);
  RUN_TEST(test_erase_root);
  RUN_TEST(test_minmax_suite);
  RUN_TEST(test_to_array_suite);
  RUN_TEST(test_find_erase_rand);
  return UNITY_END();
}
