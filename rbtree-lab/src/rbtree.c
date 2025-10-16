#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nil = calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  // point root and nil to nil
  p->root = nil;
  p->nil = nil;
  return p;
}

void delete_rbtree_rec(node_t *root, node_t *nil) {
  if (root == nil || root == NULL) {
    return;
  }

  delete_rbtree_rec(root->left, nil);
  delete_rbtree_rec(root->right, nil);
  free(root);
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  if (t == NULL)
    return;

  delete_rbtree_rec(t->root, t->nil);
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {

    // TODO: implement find
    if (t == NULL || t->root == t->nil)
            return t->nil; // 빈 트리일
    node_t *node = t->root;

    while (node != t->nil) {
        if (node->key == key) return node;
        else if(node->key > key) node = node->left;
        else node = node->right;
    }

  return t->nil;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  if (t == NULL || t->root == t->nil)
          return t->nil; // 빈 트리일 때 nil 반환

  node_t *node = t->root;
  while (node->left != t->nil) {
         node = node->left;
  }

  return node;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  if (t == NULL || t->root == t->nil)
          return t->nil; // 빈 트리일 때 nil 반환

  node_t *node = t->root;
  while (node->right != t->nil) {
         node = node->right;
  }

  return node;

}


int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

void left_rotate(rbtree *t, node_t *x) {
  if (t == NULL || x == NULL || x->right == t->nil)
    return;

  // save the x_right
  node_t *x_right = x->right;

  x->right = x_right->left;
  if (x_right->left != t->nil){
      x_right->left->parent = x;
  }

  // y를 x의 부모 자리에 연결 (루트/비루트 공통 처리)
  x_right->parent = x->parent;

  // root
  if(x_right->parent == t->nil){
      t->root = x_right;
  }
  // non-root but x's parent's left is x
  else if (x->parent->left == x){
      x->parent->left = x_right;
  }
  else{
      x->parent->right = x_right;
  }


  // x의 자리에 x_right
  x_right->left = x;
  x->parent = x_right;

};


void right_rotate(rbtree *t, node_t *y) {
    if (t == NULL || y == NULL || y->left == t->nil)
      return;

    // save the y_left
    node_t *y_left = y->left;

    y->left = y_left->right;
    if (y_left->right != t->nil){
        y_left->right->parent = y;
    }

    // y를 x의 부모 자리에 연결 (루트/비루트 공통 처리)
    y_left->parent = y->parent;

    // root
    if(y_left->parent == t->nil){
        t->root = y_left;
    }
    // non-root but x's parent's left is x
    else if (y->parent->right== y){
        y->parent->right = y_left;
    }
    else{
        y->parent->left = y_left;
    }


    // x의 자리에 x_right
    y_left->right = y;
    y->parent = y_left;
};
