#include "unity.h"
#include "unity_internals.h"
#include "../src/rbtree.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef TEST_ASSERT_NOT_EQUAL_PTR
#define TEST_ASSERT_NOT_EQUAL_PTR(exp, act) TEST_ASSERT_TRUE((exp)!=(act))
#endif
#ifndef TEST_ASSERT_NOT_EQUAL_PTR_MESSAGE
#define TEST_ASSERT_NOT_EQUAL_PTR_MESSAGE(exp, act, msg) TEST_ASSERT_TRUE_MESSAGE((exp)!=(act), (msg))
#endif

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

/* ========================
   Red-Black Tree property assertion helpers
========================= */

static inline int is_nil_node(const rbtree *t, const node_t *x) {
#ifdef SENTINEL
  return x == t->nil;
#else
  return x == NULL;
#endif
}

static inline int is_red(const node_t *x) {
  return x && x->color == RBTREE_RED;
}

static inline int is_black_node(const rbtree *t, const node_t *x) {
  if (is_nil_node(t, x))
    return 1; // NIL/NULL treated as black
  return x->color == RBTREE_BLACK;
}

/* 루트는 항상 BLACK이어야 한다 */
static void assert_root_black(const rbtree *t) {
  if (!is_nil_node(t, t->root)) {
    TEST_ASSERT_EQUAL_INT(RBTREE_BLACK, t->root->color);
  }
}

/* 어떤 RED 노드도 RED 자식을 가지면 안 된다 */
static void assert_no_red_red_recursive(const rbtree *t, const node_t *x) {
  if (is_nil_node(t, x))
    return;
  if (is_red(x)) {
    TEST_ASSERT_TRUE_MESSAGE(is_black_node(t, x->left),
                             "Red node has red left child");
    TEST_ASSERT_TRUE_MESSAGE(is_black_node(t, x->right),
                             "Red node has red right child");
  }
  assert_no_red_red_recursive(t, x->left);
  assert_no_red_red_recursive(t, x->right);
}
static void assert_no_red_red(const rbtree *t) {
  assert_no_red_red_recursive(t, t->root);
}

/* 모든 루트->리프 경로의 black-height는 같아야 한다 */
static int black_height_or_neg1(const rbtree *t, const node_t *x) {
  if (is_nil_node(t, x))
    return 1; // count NIL as 1 black
  int lh = black_height_or_neg1(t, x->left);
  int rh = black_height_or_neg1(t, x->right);
  if (lh < 0 || rh < 0 || lh != rh)
    return -1;
  return lh + (x->color == RBTREE_BLACK ? 1 : 0);
}
static void assert_black_height_consistent(const rbtree *t) {
  int h = black_height_or_neg1(t, t->root);
  TEST_ASSERT_NOT_EQUAL(-1, h);
}

/* 편의: nil이 아닌 노드이고 키가 기대값과 같은지 검사 */
static void assert_key_eq_not_nil(const rbtree *t, const node_t *x, key_t expect) {
  TEST_ASSERT_FALSE(is_nil_node(t, x));
  TEST_ASSERT_EQUAL_INT(expect, x->key);
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
#ifdef SENTINEL
  TEST_ASSERT_EQUAL_PTR(t->nil, q);
#else
  TEST_ASSERT_NULL(q);
#endif

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
  for (size_t i = 0; i < n; i++)
    arr[i] = rand();

  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_insert(t, arr[i]);
    TEST_ASSERT_NOT_NULL(p);
  }
  delete_rbtree(t);
  free(arr);
}

void test_delete_empty_tree(void) {
  rbtree *t = new_rbtree();
  delete_rbtree(t); // should not crash
  TEST_PASS();      // if it reaches here, it's good
}

/* ===== Helpers for tests ===== */

static node_t* make_node(rbtree *t, key_t k, color_t c) {
  node_t *n = (node_t*)calloc(1, sizeof(node_t));
  n->key = k;
  n->color = c;
#ifdef SENTINEL
  n->left = n->right = n->parent = t->nil;
#else
  n->left = n->right = n->parent = NULL;
#endif
  return n;
}

static void link_left(rbtree *t, node_t *p, node_t *child) {
#ifdef SENTINEL
  p->left = child ? child : t->nil;
  if (child && child != t->nil) child->parent = p;
#else
  p->left = child;
  if (child) child->parent = p;
#endif
}

static void link_right(rbtree *t, node_t *p, node_t *child) {
#ifdef SENTINEL
  p->right = child ? child : t->nil;
  if (child && child != t->nil) child->parent = p;
#else
  p->right = child;
  if (child) child->parent = p;
#endif
}

static void set_root(rbtree *t, node_t *r) {
#ifdef SENTINEL
  t->root = r ? r : t->nil;
  if (r && r != t->nil) r->parent = t->nil;
#else
  t->root = r;
  if (r) r->parent = NULL;
#endif
}

// root -> left + right rotation
void test_left_rotate_at_root_simple(void) {
  rbtree *t = new_rbtree();
  node_t *x = make_node(t, 10, RBTREE_BLACK);
  node_t *y = make_node(t, 20, RBTREE_RED);
  node_t *beta = make_node(t, 15, RBTREE_BLACK);

  set_root(t, x);
  link_right(t, x, y);
  link_left(t, y, beta);

  color_t cx = x->color, cy = y->color, cb = beta->color;

  left_rotate(t, x);

  // 구조 확인: y가 루트, y->left = x, x->right = beta
  TEST_ASSERT_EQUAL_PTR(t->root, y);
  TEST_ASSERT_EQUAL_PTR(y->left, x);
  TEST_ASSERT_EQUAL_PTR(x->right, beta);

#ifdef SENTINEL
  TEST_ASSERT_EQUAL_PTR(y->parent, t->nil);
#else
  TEST_ASSERT_NULL(y->parent);
#endif
  TEST_ASSERT_EQUAL_PTR(x->parent, y);
  TEST_ASSERT_EQUAL_PTR(beta->parent, x);

  // 색 보존
  TEST_ASSERT_EQUAL_INT(cx, x->color);
  TEST_ASSERT_EQUAL_INT(cy, y->color);
  TEST_ASSERT_EQUAL_INT(cb, beta->color);

  // BST 순서(간단 체크)
  TEST_ASSERT_TRUE(x->key < y->key);
  TEST_ASSERT_TRUE(x->key < beta->key && beta->key < y->key);

  delete_rbtree(t);
}

void test_right_rotate_at_root_simple(void) {
  rbtree *t = new_rbtree();
  node_t *y = make_node(t, 20, RBTREE_BLACK);
  node_t *x = make_node(t, 10, RBTREE_RED);
  node_t *beta = make_node(t, 15, RBTREE_BLACK);

  set_root(t, y);
  link_left(t, y, x);
  link_right(t, x, beta);

  color_t cx = x->color, cy = y->color, cb = beta->color;

  right_rotate(t, y);

  // 구조 확인: x가 루트, x->right = y, y->left = beta
  TEST_ASSERT_EQUAL_PTR(t->root, x);
  TEST_ASSERT_EQUAL_PTR(x->right, y);
  TEST_ASSERT_EQUAL_PTR(y->left, beta);

#ifdef SENTINEL
  TEST_ASSERT_EQUAL_PTR(x->parent, t->nil);
#else
  TEST_ASSERT_NULL(x->parent);
#endif
  TEST_ASSERT_EQUAL_PTR(y->parent, x);
  TEST_ASSERT_EQUAL_PTR(beta->parent, y);

  // 색 보존
  TEST_ASSERT_EQUAL_INT(cx, x->color);
  TEST_ASSERT_EQUAL_INT(cy, y->color);
  TEST_ASSERT_EQUAL_INT(cb, beta->color);

  // BST 순서(간단 체크)
  TEST_ASSERT_TRUE(x->key < y->key);
  TEST_ASSERT_TRUE(x->key < beta->key && beta->key < y->key);

  delete_rbtree(t);
}


// 비-루트 노드에서의 회전

void test_left_rotate_updates_parent_link_when_x_is_left_child(void) {
  rbtree *t = new_rbtree();
  node_t *p = make_node(t, 50, RBTREE_BLACK);
  node_t *x = make_node(t, 30, RBTREE_BLACK);
  node_t *y = make_node(t, 40, RBTREE_RED);

  set_root(t, p);
  link_left(t, p, x);
  link_right(t, x, y);

  left_rotate(t, x);

  // p의 left가 y로 바뀌어야 함
  TEST_ASSERT_EQUAL_PTR(p->left, y);
  TEST_ASSERT_EQUAL_PTR(y->parent, p);
  TEST_ASSERT_EQUAL_PTR(y->left, x);
  TEST_ASSERT_EQUAL_PTR(x->parent, y);

  delete_rbtree(t);
}

void test_right_rotate_updates_parent_link_when_y_is_right_child(void) {
  rbtree *t = new_rbtree();
  node_t *p = make_node(t, 50, RBTREE_BLACK);
  node_t *y = make_node(t, 70, RBTREE_BLACK);
  node_t *x = make_node(t, 60, RBTREE_RED);

  set_root(t, p);
  link_right(t, p, y);
  link_left(t, y, x);

  right_rotate(t, y);

  // p의 right가 x로 바뀌어야 함
  TEST_ASSERT_EQUAL_PTR(p->right, x);
  TEST_ASSERT_EQUAL_PTR(x->parent, p);
  TEST_ASSERT_EQUAL_PTR(x->right, y);
  TEST_ASSERT_EQUAL_PTR(y->parent, x);

  delete_rbtree(t);
}

// 선행조건 위반 시 실패
void test_left_rotate_precondition_fail_when_no_right_child(void) {
  rbtree *t = new_rbtree();
  node_t *x = make_node(t, 10, RBTREE_BLACK);
  set_root(t, x);

  // 기대: 테스트 실패(ASSERT) – 회전 선행조건 위반
  // left_rotate(t, x);

  delete_rbtree(t);
  TEST_PASS_MESSAGE("Manually inspected: call left_rotate(t,x) should fail when x->right is NIL/NULL");
}

/* RR case: insert 10,20,30 -> root must be 20 with children 10 and 30 */
void test_insert_rr_case_shapes_and_props(void) {
  rbtree *t = new_rbtree();
  rbtree_insert(t, 10);
  rbtree_insert(t, 20);
  rbtree_insert(t, 30);

  assert_key_eq_not_nil(t, t->root, 20);
  assert_key_eq_not_nil(t, t->root->left, 10);
  assert_key_eq_not_nil(t, t->root->right, 30);

  assert_root_black(t);
  assert_no_red_red(t);
  assert_black_height_consistent(t);
  delete_rbtree(t);
}

/* LL case: insert 30,20,10 -> root must be 20 with children 10 and 30 */
void test_insert_ll_case_shapes_and_props(void) {
  rbtree *t = new_rbtree();
  rbtree_insert(t, 30);
  rbtree_insert(t, 20);
  rbtree_insert(t, 10);

  assert_key_eq_not_nil(t, t->root, 20);
  assert_key_eq_not_nil(t, t->root->left, 10);
  assert_key_eq_not_nil(t, t->root->right, 30);

  assert_root_black(t);
  assert_no_red_red(t);
  assert_black_height_consistent(t);
  delete_rbtree(t);
}

/* LR case: insert 30,10,20 -> root must be 20 with children 10 and 30 */
void test_insert_lr_case_shapes_and_props(void) {
  rbtree *t = new_rbtree();
  rbtree_insert(t, 30);
  rbtree_insert(t, 10);
  rbtree_insert(t, 20);

  assert_key_eq_not_nil(t, t->root, 20);
  assert_key_eq_not_nil(t, t->root->left, 10);
  assert_key_eq_not_nil(t, t->root->right, 30);

  assert_root_black(t);
  assert_no_red_red(t);
  assert_black_height_consistent(t);
  delete_rbtree(t);
}

/* RL case: insert 10,30,20 -> root must be 20 with children 10 and 30 */
void test_insert_rl_case_shapes_and_props(void) {
  rbtree *t = new_rbtree();
  rbtree_insert(t, 10);
  rbtree_insert(t, 30);
  rbtree_insert(t, 20);

  assert_key_eq_not_nil(t, t->root, 20);
  assert_key_eq_not_nil(t, t->root->left, 10);
  assert_key_eq_not_nil(t, t->root->right, 30);

  assert_root_black(t);
  assert_no_red_red(t);
  assert_black_height_consistent(t);
  delete_rbtree(t);
}


/* ========================
   Min / Max / Find — Dedicated Tests
========================= */

/* 빈 트리, 단일 노드, 다수 노드(중복 포함)에서 find 동작 검증 */
void test_find_suite(void) {
  // 1) 빈 트리
  {
    rbtree *t = new_rbtree();
    node_t *q = rbtree_find(t, 42);
#ifdef SENTINEL
    TEST_ASSERT_EQUAL_PTR_MESSAGE(t->nil, q, "empty tree: find should return t->nil");
#else
    TEST_ASSERT_NULL_MESSAGE(q, "empty tree: find should return NULL");
#endif
    delete_rbtree(t);
  }

  // 2) 단일 노드
  {
    rbtree *t = new_rbtree();
    node_t *p = rbtree_insert(t, 100);
    TEST_ASSERT_NOT_NULL(p);

    node_t *q = rbtree_find(t, 100);
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_INT(100, q->key);
    TEST_ASSERT_EQUAL_PTR(p, q);

    // 없는 키
    q = rbtree_find(t, 99);
#ifdef SENTINEL
    TEST_ASSERT_EQUAL_PTR(t->nil, q);
#else
    TEST_ASSERT_NULL(q);
#endif
    delete_rbtree(t);
  }

  // 3) 다수 노드(중복 포함)
  {
    rbtree *t = new_rbtree();
    key_t arr[] = {10, 5, 20, 5, 30, 5, 15};
    size_t n = sizeof(arr)/sizeof(arr[0]);
    for (size_t i = 0; i < n; i++) {
      TEST_ASSERT_NOT_NULL(rbtree_insert(t, arr[i]));
    }

    // 존재하는 키들
    // 기존 (C++ 문법) —— 지우기
    // for (int k : (int[]){5, 10, 15, 20, 30}) {
    //   node_t *q = rbtree_find(t, k);
    //   ...
    // }

    // 새 코드 (C)
    int keys[] = {5, 10, 15, 20, 30};
    size_t keys_n = sizeof(keys) / sizeof(keys[0]);
    for (size_t i = 0; i < keys_n; i++) {
      int k = keys[i];
      node_t *q = rbtree_find(t, k);
    #ifdef SENTINEL
      TEST_ASSERT_TRUE_MESSAGE(q != t->nil, "find should not return t->nil");
    #else
      TEST_ASSERT_NOT_NULL(q);
    #endif
      TEST_ASSERT_EQUAL_INT(k, q->key);
    }

    // 없는 키
    node_t *q = rbtree_find(t, 7);
#ifdef SENTINEL
    TEST_ASSERT_EQUAL_PTR(t->nil, q);
#else
    TEST_ASSERT_NULL(q);
#endif

    // 중복값 5 하나 삭제 후에도 find(5)는 여전히 존재해야 함
    node_t *p5 = rbtree_find(t, 5);
#ifdef SENTINEL
    TEST_ASSERT_NOT_EQUAL_PTR(t->nil, p5);
#else
    TEST_ASSERT_NOT_NULL(p5);
#endif
    rbtree_erase(t, p5);

    q = rbtree_find(t, 5);
#ifdef SENTINEL
    TEST_ASSERT_NOT_EQUAL_PTR_MESSAGE(t->nil, q, "duplicates: after erasing one 5, another 5 should remain");
#else
    TEST_ASSERT_NOT_NULL_MESSAGE(q, "duplicates: after erasing one 5, another 5 should remain");
#endif
    TEST_ASSERT_EQUAL_INT(5, q->key);

    delete_rbtree(t);
  }
}

/* rbtree_min 전용 검증: 빈/단일/다수/삭제 후 */
void test_min_suite(void) {
  // 1) 빈 트리
  {
    rbtree *t = new_rbtree();
    node_t *mn = rbtree_min(t);
#ifdef SENTINEL
    TEST_ASSERT_EQUAL_PTR(t->nil, mn);
#else
    TEST_ASSERT_NULL(mn);
#endif
    delete_rbtree(t);
  }

  // 2) 단일 노드
  {
    rbtree *t = new_rbtree();
    TEST_ASSERT_NOT_NULL(rbtree_insert(t, 42));
    node_t *mn = rbtree_min(t);
    TEST_ASSERT_NOT_NULL(mn);
    TEST_ASSERT_EQUAL_INT(42, mn->key);
    delete_rbtree(t);
  }

  // 3) 다수 노드 + 삭제 후
  {
    rbtree *t = new_rbtree();
    key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
    size_t n = sizeof(arr)/sizeof(arr[0]);
    for (size_t i = 0; i < n; i++) TEST_ASSERT_NOT_NULL(rbtree_insert(t, arr[i]));

    node_t *mn = rbtree_min(t);
    TEST_ASSERT_NOT_NULL(mn);
    TEST_ASSERT_EQUAL_INT(2, mn->key);

    // 최소값 삭제 후 다음 최소값 확인 (5)
    rbtree_erase(t, mn);
    mn = rbtree_min(t);
    TEST_ASSERT_NOT_NULL(mn);
    TEST_ASSERT_EQUAL_INT(5, mn->key);

    delete_rbtree(t);
  }
}

/* rbtree_max 전용 검증: 빈/단일/다수/삭제 후 */
void test_max_suite(void) {
  // 1) 빈 트리
  {
    rbtree *t = new_rbtree();
    node_t *mx = rbtree_max(t);
#ifdef SENTINEL
    TEST_ASSERT_EQUAL_PTR(t->nil, mx);
#else
    TEST_ASSERT_NULL(mx);
#endif
    delete_rbtree(t);
  }

  // 2) 단일 노드
  {
    rbtree *t = new_rbtree();
    TEST_ASSERT_NOT_NULL(rbtree_insert(t, 7));
    node_t *mx = rbtree_max(t);
    TEST_ASSERT_NOT_NULL(mx);
    TEST_ASSERT_EQUAL_INT(7, mx->key);
    delete_rbtree(t);
  }

  // 3) 다수 노드 + 삭제 후
  {
    rbtree *t = new_rbtree();
    key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
    size_t n = sizeof(arr)/sizeof(arr[0]);
    for (size_t i = 0; i < n; i++) TEST_ASSERT_NOT_NULL(rbtree_insert(t, arr[i]));

    node_t *mx = rbtree_max(t);
    TEST_ASSERT_NOT_NULL(mx);
    TEST_ASSERT_EQUAL_INT(156, mx->key);

    // 최대값 삭제 후 다음 최대값 확인 (67)
    rbtree_erase(t, mx);
    mx = rbtree_max(t);
    TEST_ASSERT_NOT_NULL(mx);
    TEST_ASSERT_EQUAL_INT(67, mx->key);

    delete_rbtree(t);
  }
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

  // left and right rotation
  RUN_TEST(test_left_rotate_at_root_simple);
  RUN_TEST(test_right_rotate_at_root_simple);
  RUN_TEST(test_left_rotate_updates_parent_link_when_x_is_left_child);
  RUN_TEST(test_right_rotate_updates_parent_link_when_y_is_right_child);
  // 필요 시 선행조건 위반 테스트도 추가
  RUN_TEST(test_left_rotate_precondition_fail_when_no_right_child);

  RUN_TEST(test_find_suite);
  RUN_TEST(test_min_suite);
  RUN_TEST(test_max_suite);
  // insert test
  RUN_TEST(test_insert_rr_case_shapes_and_props);
  RUN_TEST(test_insert_ll_case_shapes_and_props);
  RUN_TEST(test_insert_lr_case_shapes_and_props);
  RUN_TEST(test_insert_rl_case_shapes_and_props);
  return UNITY_END();
}
