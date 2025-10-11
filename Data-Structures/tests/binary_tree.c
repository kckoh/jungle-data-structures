#include "binary_tree.h"
#include <stdio.h>
#include <stdlib.h>

int identical(BTNode *tree1, BTNode *tree2)

{
  /* add your code here */
  // 둘 다 NULL이면 1을 반환
  if (tree1 == NULL && tree2 == NULL) {
    return 1;
  }

  // 하나만 NULL이면 0을 반환
  else if (tree1 == NULL || tree2 == NULL) {
    return 0;
  }

  // 그렇지 않고 두 노드의 값(item)이 다르면 0을 반환
  if (tree1->item != tree2->item) {
    return 0;
  }

  return identical(tree1->left, tree2->left) &&
         identical(tree1->right, tree2->right);
  // 그 외에는 왼쪽과 오른쪽을 각각 검사한 결과 반환
}

/////////////////////////////////////////////////////////////////////////////////

BTNode *createBTNode(int item) {
  BTNode *newNode = malloc(sizeof(BTNode));
  newNode->item = item;
  newNode->left = NULL;
  newNode->right = NULL;
  return newNode;
}

//////////////////////////////////////////////////////////////////////////////////

BTNode *createTree() {
  Stack stk;
  BTNode *root, *temp;
  char s;
  int item;

  stk.top = NULL;
  root = NULL;

  printf("Input an integer that you want to add to the binary tree. Any Alpha "
         "value will be treated as NULL.\n");
  printf("Enter an integer value for the root: ");
  if (scanf("%d", &item) > 0) {
    root = createBTNode(item);
    push(&stk, root);
  } else {
    scanf("%c", &s);
  }

  while ((temp = pop(&stk)) != NULL) {

    printf("Enter an integer value for the Left child of %d: ", temp->item);

    if (scanf("%d", &item) > 0) {
      temp->left = createBTNode(item);
    } else {
      scanf("%c", &s);
    }

    printf("Enter an integer value for the Right child of %d: ", temp->item);
    if (scanf("%d", &item) > 0) {
      temp->right = createBTNode(item);
    } else {
      scanf("%c", &s);
    }

    if (temp->right != NULL)
      push(&stk, temp->right);
    if (temp->left != NULL)
      push(&stk, temp->left);
  }
  return root;
}

void push(Stack *stk, BTNode *node) {
  StackNode *temp;

  temp = malloc(sizeof(StackNode));
  if (temp == NULL)
    return;
  temp->btnode = node;
  if (stk->top == NULL) {
    stk->top = temp;
    temp->next = NULL;
  } else {
    temp->next = stk->top;
    stk->top = temp;
  }
}

BTNode *pop(Stack *stk) {
  StackNode *temp, *top;
  BTNode *ptr;
  ptr = NULL;

  top = stk->top;
  if (top != NULL) {
    temp = top->next;
    ptr = top->btnode;

    stk->top = temp;
    free(top);
    top = NULL;
  }
  return ptr;
}

void printTree(BTNode *node) {
  if (node == NULL)
    return;

  printTree(node->left);
  printf("%d ", node->item);
  printTree(node->right);
}

void removeAll(BTNode **node) {
  if (*node != NULL) {
    removeAll(&((*node)->left));
    removeAll(&((*node)->right));
    free(*node);
    *node = NULL;
  }
}
