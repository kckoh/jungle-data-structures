#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/rbtree.h"

/* ===== Sentinel-aware helpers ===== */
static inline node_t* RB_NIL(const rbtree *t) {
#ifdef SENTINEL
  return t->nil;
#else
  return NULL;
#endif
}

static inline bool is_nil(const rbtree *t, const node_t *x) {
#ifdef SENTINEL
  return x == t->nil;
#else
  return x == NULL;
#endif
}

/* ===== Basic init test ===== */
void test_init(void) {
  rbtree *t = new_rbtree();
  assert(t != NULL);
#ifdef SENTINEL
  assert(t->nil != NULL);
  assert(t->root == t->nil);
#else
  assert(t->root == NULL);
#endif
  delete_rbtree(t);
}

/* ===== Insert single ===== */
void test_insert_single(const key_t key) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);
  assert(!is_nil(t, p));
  assert(t->root == p);
  assert(p->key == key);
#ifdef SENTINEL
  assert(p->left == t->nil);
  assert(p->right == t->nil);
  assert(p->parent == t->nil);
#else
  assert(p->left == NULL);
  assert(p->right == NULL);
  assert(p->parent == NULL);
#endif
  delete_rbtree(t);
}

/* ===== Find single ===== */
void test_find_single(const key_t key, const key_t wrong_key) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);

  node_t *q = rbtree_find(t, key);
  assert(!is_nil(t, q));
  assert(q->key == key);
  assert(q == p);

  q = rbtree_find(t, wrong_key);
  assert(is_nil(t, q));

  delete_rbtree(t);
}

/* ===== Erase root ===== */
void test_erase_root(const key_t key) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);
  assert(!is_nil(t, p));
  assert(t->root == p);
  assert(p->key == key);

  rbtree_erase(t, p);
#ifdef SENTINEL
  assert(t->root == t->nil);
#else
  assert(t->root == NULL);
#endif

  delete_rbtree(t);
}

/* ===== Helpers ===== */
static void insert_arr(rbtree *t, const key_t *arr, const size_t n) {
  for (size_t i = 0; i < n; i++) {
    node_t *ins = rbtree_insert(t, arr[i]);
    assert(!is_nil(t, ins));
  }
}

static int comp(const void *p1, const void *p2) {
  const key_t *e1 = (const key_t *)p1;
  const key_t *e2 = (const key_t *)p2;
  return (*e1 > *e2) - (*e1 < *e2);
}

/* ===== Min/Max ===== */
void test_minmax(key_t *arr, const size_t n) {
  assert(n > 0 && arr != NULL);

  rbtree *t = new_rbtree();
  assert(t != NULL);

  insert_arr(t, arr, n);
  assert(!is_nil(t, t->root));

  qsort((void *)arr, n, sizeof(key_t), comp);

  node_t *p = rbtree_min(t);
  assert(!is_nil(t, p));
  assert(p->key == arr[0]);

  node_t *q = rbtree_max(t);
  assert(!is_nil(t, q));
  assert(q->key == arr[n - 1]);

  rbtree_erase(t, p);
  p = rbtree_min(t);
  assert(!is_nil(t, p));
  assert(p->key == arr[1]);

  if (n >= 2) {
    rbtree_erase(t, q);
    q = rbtree_max(t);
    assert(!is_nil(t, q));
    assert(q->key == arr[n - 2]);
  }

  delete_rbtree(t);
}

/* ===== To array ===== */
void test_to_array(rbtree *t, const key_t *arr, const size_t n) {
  assert(t != NULL);

  insert_arr(t, arr, n);
  qsort((void *)arr, n, sizeof(key_t), comp);

  key_t *res = (key_t *)calloc(n, sizeof(key_t));
  assert(res != NULL);

  rbtree_to_array(t, res, n);
  for (size_t i = 0; i < n; i++) {
    assert(arr[i] == res[i]);
  }
  free(res);
}

/* ===== Multi-instance ===== */
void test_multi_instance(void) {
  rbtree *t1 = new_rbtree();
  assert(t1 != NULL);
  rbtree *t2 = new_rbtree();
  assert(t2 != NULL);

  key_t arr1[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n1 = sizeof(arr1) / sizeof(arr1[0]);
  insert_arr(t1, arr1, n1);
  qsort((void *)arr1, n1, sizeof(key_t), comp);

  key_t arr2[] = {4, 8, 10, 5, 3};
  const size_t n2 = sizeof(arr2) / sizeof(arr2[0]);
  insert_arr(t2, arr2, n2);
  qsort((void *)arr2, n2, sizeof(key_t), comp);

  key_t *res1 = (key_t *)calloc(n1, sizeof(key_t));
  key_t *res2 = (key_t *)calloc(n2, sizeof(key_t));
  assert(res1 && res2);

  rbtree_to_array(t1, res1, n1);
  for (size_t i = 0; i < n1; i++) {
    assert(arr1[i] == res1[i]);
  }

  rbtree_to_array(t2, res2, n2);
  for (size_t i = 0; i < n2; i++) {
    assert(arr2[i] == res2[i]);
  }

  free(res2);
  free(res1);
  delete_rbtree(t2);
  delete_rbtree(t1);
}

/* ===== Search tree constraint =====
   Left subtree keys <= node->key
   Right subtree keys >= node->key
*/
static bool search_traverse(const node_t *p, key_t *min, key_t *max, node_t *nil) {
  if (p == nil) return true;

  *min = *max = p->key;

  key_t l_min, l_max, r_min, r_max;
  l_min = l_max = r_min = r_max = p->key;

  const bool lr = search_traverse(p->left, &l_min, &l_max, nil);
  if (!lr || l_max > p->key) return false;

  const bool rr = search_traverse(p->right, &r_min, &r_max, nil);
  if (!rr || r_min < p->key) return false;

  *min = l_min;
  *max = r_max;
  return true;
}

void test_search_constraint(const rbtree *t) {
  assert(t != NULL);
  node_t *p = t->root;
  key_t min, max;
#ifdef SENTINEL
  node_t *nil = t->nil;
#else
  node_t *nil = NULL;
#endif
  assert(search_traverse(p, &min, &max, nil));
}

/* ===== Color/black-height constraints =====
   1) NIL are black
   2) No red node has a red child
   3) Same # of black nodes on every root->NIL path
*/
static bool touch_nil = false;
static int max_black_depth = 0;

static void init_color_traverse(void) {
  touch_nil = false;
  max_black_depth = 0;
}

static bool color_traverse(const node_t *p, const color_t parent_color,
                           const int black_depth, node_t *nil) {
  if (p == nil) {
    if (!touch_nil) {
      touch_nil = true;
      max_black_depth = black_depth;
    } else if (black_depth != max_black_depth) {
      return false;
    }
    return true;
  }
  if (parent_color == RBTREE_RED && p->color == RBTREE_RED) {
    return false;
  }
  const int next_depth = black_depth + ((p->color == RBTREE_BLACK) ? 1 : 0);
  return color_traverse(p->left, p->color, next_depth, nil) &&
         color_traverse(p->right, p->color, next_depth, nil);
}

void test_color_constraint(const rbtree *t) {
  assert(t != NULL);
#ifdef SENTINEL
  node_t *nil = t->nil;
#else
  node_t *nil = NULL;
#endif
  node_t *p = t->root;
  assert(p == nil || p->color == RBTREE_BLACK);

  init_color_traverse();
  assert(color_traverse(p, RBTREE_BLACK, 0, nil));
}

/* ===== RB global constraints ===== */
void test_rb_constraints(const key_t arr[], const size_t n) {
  rbtree *t = new_rbtree();
  assert(t != NULL);

  insert_arr(t, arr, n);
  assert(!is_nil(t, t->root));

  test_color_constraint(t);
  test_search_constraint(t);

  delete_rbtree(t);
}

/* ===== Datasets ===== */
void test_distinct_values(void) {
  const key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_rb_constraints(entries, n);
}

void test_duplicate_values(void) {
  const key_t entries[] = {10, 5, 5, 34, 6, 23, 12, 12, 6, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_rb_constraints(entries, n);
}

void test_minmax_suite(void) {
  key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_minmax(entries, n);
}

void test_to_array_suite(void) {
  rbtree *t = new_rbtree();
  assert(t != NULL);

  key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n = sizeof(entries) / sizeof(entries[0]);
  test_to_array(t, entries, n);

  delete_rbtree(t);
}

/* ===== Find + Erase combos ===== */
void test_find_erase(rbtree *t, const key_t *arr, const size_t n) {
  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    assert(!is_nil(t, p));
  }

  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_find(t, arr[i]);
    assert(!is_nil(t, p));
    assert(p->key == arr[i]);
    rbtree_erase(t, p);
  }

  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_find(t, arr[i]);
    assert(is_nil(t, p));
  }

  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    assert(!is_nil(t, p));
    node_t *q = rbtree_find(t, arr[i]);
    assert(!is_nil(t, q));
    assert(q->key == arr[i]);
    assert(p == q);
    rbtree_erase(t, p);
    q = rbtree_find(t, arr[i]);
    assert(is_nil(t, q));
  }
}

void test_find_erase_fixed(void) {
  const key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
  const size_t n = sizeof(arr) / sizeof(arr[0]);
  rbtree *t = new_rbtree();
  assert(t != NULL);

  test_find_erase(t, arr, n);

  delete_rbtree(t);
}

void test_find_erase_rand(const size_t n, const unsigned int seed) {
  srand(seed);
  rbtree *t = new_rbtree();
  key_t *arr = (key_t *)calloc(n, sizeof(key_t));
  assert(arr != NULL);

  for (size_t i = 0; i < n; i++) {
    arr[i] = (key_t)rand();
  }

  test_find_erase(t, arr, n);

  free(arr);
  delete_rbtree(t);
}

/* ===== Main runner ===== */
int main(void) {
  test_init();
  test_insert_single(1024);
  test_find_single(512, 1024);
  test_erase_root(128);
  test_find_erase_fixed();
  test_minmax_suite();
  test_to_array_suite();
  test_distinct_values();
  test_duplicate_values();
  test_multi_instance();
  test_find_erase_rand(10000, 17);
  printf("Passed all tests!\n");
  return 0;
}
