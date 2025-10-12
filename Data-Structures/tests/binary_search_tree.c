#include "binary_search_tree.h"
#include <math.h>

// TODO: I need to fix the push duplicate
int findLengthQueue(Queue *q) {
  if (q == NULL || isEmpty(q->head)) {
    return 0;
  }
  QueueNode *node = q->head;
  int size = 0;
  while (node != NULL) {
    node = node->nextPtr;
    size++;
  }
  return size;
}

void levelOrderTraversal(BSTNode *root) {

  if (root == NULL) {
    return;
  }

  Queue q;
  q.head = NULL;
  q.tail = NULL;
  enqueue(&q.head, &q.tail, root);

  while (isEmpty(q.head) != 1) {
    int size = findLengthQueue(&q);
    for (int i = 0; i < size; i++) {

      BSTNode *pop = dequeue(&q.head, &q.tail);
      printf("%d ", pop->item);
      if (pop->left != NULL) {
        enqueue(&q.head, &q.tail, pop->left);
      }

      if (pop->right != NULL) {
        enqueue(&q.head, &q.tail, pop->right);
      }
    }
  }
}

// prints the inorder traversal of a binary search tree using a stack. Note that
// you should only use push() or pop() operations when you add or remove
// integers from the stack. Remember to empty the stack at the beginning, if the
// stack is not empty.
void inOrderIterative(BSTNode *root) {
  if (root == NULL) {
    return;
  }
  Stack s;
  s.top = NULL;


  BSTNode *cur = root;

  while (cur != NULL || !isEmptyStack(&s)) {

      // 왼쪽 끝까지
      while (cur != NULL) {
          push(&s, cur);
          cur = cur->left;
      }

      // 하나 꺼내서 방문
      BSTNode *node = pop(&s);
      printf("%d ",node->item);

      // 오른쪽으로 이동
      cur = node->right;
  }
}

void preOrderIterative(BSTNode *root) {
    if (root == NULL) {
      return;
    }

    Stack s;
    s.top = NULL;

    BSTNode *cur = NULL;
    push(&s,root);

    while ( !isEmptyStack(&s)) {

        // 하나 꺼내서 방문
        BSTNode *node = pop(&s);
        printf("%d ",node->item);

        if(node->right != NULL){
            push(&s, node->right);
        }

        if (node ->left != NULL){
            push(&s, node->left);
        }

    }
}

void postOrderIterativeS1(BSTNode *root) {

}

void postOrderIterativeS2(BSTNode *root) { /* add your code here */ }

///////////////////////////////////////////////////////////////////////////////

void insertBSTNode(BSTNode **node, int value) {
  if (*node == NULL) {
    *node = malloc(sizeof(BSTNode));

    if (*node != NULL) {
      (*node)->item = value;
      (*node)->left = NULL;
      (*node)->right = NULL;
    }
  } else {
    if (value < (*node)->item) {
      insertBSTNode(&((*node)->left), value);
    } else if (value > (*node)->item) {
      insertBSTNode(&((*node)->right), value);
    } else
      return;
  }
}

//////////////////////////////////////////////////////////////////////////////////

// enqueue node
void enqueue(QueueNode **headPtr, QueueNode **tailPtr, BSTNode *node) {
  // dynamically allocate memory
  QueueNode *newPtr = malloc(sizeof(QueueNode));

  // if newPtr does not equal NULL
  if (newPtr != NULL) {
    newPtr->data = node;
    newPtr->nextPtr = NULL;

    // if queue is empty, insert at head
    if (isEmpty(*headPtr)) {
      *headPtr = newPtr;
    } else { // insert at tail
      (*tailPtr)->nextPtr = newPtr;
    }

    *tailPtr = newPtr;
  } else {
    printf("Node not inserted");
  }
}

BSTNode *dequeue(QueueNode **headPtr, QueueNode **tailPtr) {
  BSTNode *node = (*headPtr)->data;
  QueueNode *tempPtr = *headPtr;
  *headPtr = (*headPtr)->nextPtr;

  if (*headPtr == NULL) {
    *tailPtr = NULL;
  }

  free(tempPtr);

  return node;
}

int isEmpty(QueueNode *head) { return head == NULL; }

void removeAll(BSTNode **node) {
  if (*node != NULL) {
    removeAll(&((*node)->left));
    removeAll(&((*node)->right));
    free(*node);
    *node = NULL;
  }
}

void push(Stack *stack, BSTNode *node) {
  StackNode *temp;

  temp = malloc(sizeof(StackNode));

  if (temp == NULL)
    return;
  temp->data = node;

  if (stack->top == NULL) {
    stack->top = temp;
    temp->next = NULL;
  } else {
    temp->next = stack->top;
    stack->top = temp;
  }
}

BSTNode *pop(Stack *s) {
  StackNode *temp, *t;
  BSTNode *ptr;
  ptr = NULL;

  t = s->top;
  if (t != NULL) {
    temp = t->next;
    ptr = t->data;

    s->top = temp;
    free(t);
    t = NULL;
  }

  return ptr;
}

BSTNode *peek(Stack *s) {
  StackNode *temp;
  temp = s->top;
  if (temp != NULL)
    return temp->data;
  else
    return NULL;
}

int isEmptyStack(Stack *s) {
  if (s->top == NULL)
    return 1;
  else
    return 0;
}

void removeAllStack(BSTNode **node) {
  if (*node != NULL) {
    removeAll(&((*node)->left));
    removeAll(&((*node)->right));
    free(*node);
    *node = NULL;
  }
}
