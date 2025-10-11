#include "binary_search_tree.h"

int findLengthQueue(Queue *q){
    if (q == NULL || isEmpty(q->head)){
        return 0;
    }
    QueueNode *node = q->head;
    int size = 0;
    while (node != NULL){
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
        printf("%d ",pop->item);
        if(pop->left != NULL){
            enqueue(&q.head, &q.tail, pop->left);
        }

        if(pop->right != NULL){
            enqueue(&q.head, &q.tail, pop->right);
        }

    }
  }

}

void inOrderIterative(BSTNode *root) { /* add your code here */ }

void preOrderIterative(BSTNode *root) {}

void postOrderIterativeS1(BSTNode *root) { /* add your code here */ }

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
