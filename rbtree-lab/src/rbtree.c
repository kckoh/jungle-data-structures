#include "rbtree.h"
#include <stdlib.h>
#include <sys/__endian.h>
#include <sys/stdio.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  nil->left = nil;
  nil->right = nil;
  nil->parent = nil;

  p->nil = nil;

#ifdef SENTINEL
  p->root = p->nil;   // ← SENTINEL 모드: root는 nil
#else
  p->root = NULL;     // ← 비-센티널 모드: root는 NULL
#endif

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

static node_t *newNode(rbtree *t, key_t key){
    node_t *z = (node_t*) malloc(sizeof(node_t));

    z->key = key;
    z->color = RBTREE_RED;
    z->left = t->nil;
    z->right = t->nil;
    z->parent = t->nil;
    return z;
}

void rbtree_fixup(rbtree *t, node_t *z){
    while (z->parent->color == RBTREE_RED){
        if (z->parent == z->parent->parent->left){ //is z's parent a left child?
            node_t *z_uncle = z->parent->parent->right;

            // CASE 1
            // 삼촌 레드
            // 조치: recolor + 위로 올리기 -> while loop
            if (z_uncle->color == RBTREE_RED){
                z->parent->color = RBTREE_BLACK;
                z_uncle->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }

            else{
                // CASE 2
                // 삼촌 BLACK + z가 반대방향
                // 조치: 회전
                if(z==z->parent->right){
                    z = z->parent;
                    left_rotate(t, z);
                }

                // CASE 3
                // 삼촌 BLACK + z가 같은방향
                // 조치: 회전 + 색 교환
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                right_rotate(t, z->parent->parent);
            }
        }
        // same as above except with right and left exchanged
        else{
            node_t *z_uncle = z->parent->parent->left;
            // CASE 1
            if (z_uncle->color == RBTREE_RED){
                z->parent->color = RBTREE_BLACK;
                z_uncle->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else {
                // CASE 2
                if (z== z->parent->left){
                    z =z->parent;
                    right_rotate(t, z);
                }

                // CASE 3
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                left_rotate(t,z->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;

}


node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *x = t->root;
  node_t *y = t->nil;
  node_t *z = newNode(t, key);

  while ( x != t->nil){ // descend until reaching the sentinel
      y = x;
      if (z->key < x->key) x = x->left;
      else x = x->right;
  }

  z->parent = y; // found the location - insert z with parent y

  if (y == t->nil) t->root = z; // t was empty
  else if (z->key < y->key) y->left = z;
  else y->right = z;

  //z.left = t.nil
  // z.right = t.nil
  // z.color = red
  rbtree_fixup(t,z);
  return z;
}



node_t *rbtree_find(const rbtree *t, const key_t key) {

    // TODO: implement find
#ifdef SENTINEL
    if (t == NULL || t->root == t->nil)
            return t->nil; // 빈 트리일
#else
    if (t == NULL || t->root == t->nil)
            return NULL; // 빈 트리일
#endif
    node_t *node = t->root;
    // if (node->left == t->nil && node->right == t->nil) return node;


    while (node != t->nil) {
        if (node->key == key) return node;
        else if(node->key > key) node = node->left;
        else node = node->right;
    }

#ifdef SENTINEL
  return t->nil;
#else
  return NULL;
#endif
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

node_t *tree_minimum(rbtree *t, node_t *x) {
    while (x->left != t->nil)
        x = x->left;
    return x;
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

// 트리에서 한 노드를 다른 노드로 대체
void rbtree_transplant(rbtree *t, node_t *u, node_t *v){
    if(u->parent == t->nil) t->root = v;
    else if (u == u->parent ->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *x){
    while(x != t->root && x->color ==RBTREE_BLACK){
        if (x == x->parent->left){
            node_t *w = x->parent->right;

            if( w->color == RBTREE_RED){
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                left_rotate(t, x->parent);
                w = w->parent->right;
            }
            if(w->left->color == RBTREE_BLACK && w->right->color ==RBTREE_BLACK){
                w->color = RBTREE_RED;
                x = x->parent;
            }
            else{
                if (w->right->color == RBTREE_BLACK){
                    w->left->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    right_rotate(t, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->right->color = RBTREE_BLACK;
                left_rotate(t, x->parent);
                x = t->root;
            }

        }
        else{
            node_t *w = x->parent->left;

            if (w->color == RBTREE_RED){
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t, x->parent);
                w = x->parent->left;
            }

            if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
                w->color = RBTREE_RED;
                x = x->parent;
            }
            else{
                if (w ->left->color == RBTREE_BLACK){
                    w->right->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    left_rotate(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->left->color = RBTREE_BLACK;
                right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }

    x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *z) {
    node_t *y = z;
    node_t *x;
    color_t y_original_color = y->color;
    if (z->left == t->nil) {
        x = z->right;
        rbtree_transplant(t, z, z->right);
    }

    else if(z->right == t->nil){
        x = z->left;
        rbtree_transplant(t, z, z->left);
    }

    else {
        y = tree_minimum(t,z->right);
        y_original_color = y->color;
        x = y->right;
        if (y != z->right){
           rbtree_transplant(t, y, y->right);
          y->right = z->right;
         y->right->parent = y;
        }
        else x->parent = y;
        rbtree_transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == RBTREE_BLACK) rbtree_delete_fixup(t, x);

    free(z);
    return 0;
}

void inorder_traversal(node_t *node, key_t *arr, int *index, const rbtree *t) {
  if (node == t->nil ) return;
  inorder_traversal(node->left, arr, index,t);
  arr[(*index)++] = node->key;
  inorder_traversal(node->right, arr, index,t);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  int index = 0;
  inorder_traversal(t->root, arr, &index,t);
  return index;
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
