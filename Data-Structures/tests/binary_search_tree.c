#include "binary_search_tree.h"
#include <math.h>
#include <stdio.h>

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

// very difficult
void postOrderIterativeS1(BSTNode *root) {
    if (root == NULL) {
      return;
    }
    Stack s;
    s.top = NULL;

    BSTNode *cur = root;
    BSTNode *lastVisited = NULL;
    while (cur != NULL || !isEmptyStack(&s)) {

        // 왼쪽 끝까지
        while (cur != NULL) {
            push(&s, cur);
            cur = cur->left;
        }

        // 하나 peek
        BSTNode * p = peek(&s);
        if (p->right != NULL && lastVisited != p->right){
            cur = p->right;
        }
        // 방문 and p->right = NULL
        else{
            BSTNode *popped = pop(&s);
           printf("%d ", popped->item);
           lastVisited = popped;
        }

    }


}

void postOrderIterativeS2(BSTNode *root) {

    if (root == NULL) {
      return;
    }

    Stack s1;
    s1.top = NULL;

    Stack s2;
    s2.top = NULL;

    BSTNode *cur = NULL;
    push(&s1,root);

    while ( !isEmptyStack(&s1)) {

        // 하나 꺼내서 방문
        BSTNode *node = pop(&s1);
        push(&s2,node);

        if (node ->left != NULL){
            push(&s1, node->left);
        }

        if(node->right != NULL){
            push(&s1, node->right);
        }
    }

    while ( !isEmptyStack(&s2)) {
        // 하나 꺼내서 방문
        BSTNode *node = pop(&s2);
        printf("%d ",node->item);
    }

}

/* Given a binary search tree and a key, this function
   deletes the key and returns the new root. Make recursive function. */
BSTNode* removeNodeFromTree(BSTNode *root, int value)
{
	/* add your code here */
	// two base case -> if null -> return;
	if (root == NULL){
	    return NULL;
	}

	if (value < root->item) {
        root->left  = removeNodeFromTree(root->left, value);   // ← 재연결
    } else if (value > root->item) {
        root->right = removeNodeFromTree(root->right, value);  // ← 재연결
    } else {
        // 둘다 없음
        if (root->left == NULL && root->right == NULL){
            // free
            free(root);
            return NULL;
        }

        // 둘다 있음 -> 전임자/후임자 절차 중 하나 적용
        else if (root->left && root->right){
            // 후임자: 오른쪽 서브트리의 최소값
            BSTNode *succ = root->right;
            while (succ->left != NULL){
                succ = succ->left;
            }
            // 값 복사
            root->item = succ->item;
            // 후임자를 '오른쪽 서브트리'에서 삭제
            root->right = removeNodeFromTree(root->right, succ->item);
            return root;
        }

        // 오른쪽 없음 -> 왼쪽 올리기
        else if (root->left && root->right == NULL){
            // 저장
           BSTNode *left = root->left;
            free(root);
            return left;
        }

        // 왼쪽 없음 -> 오른쪽 올리기
        else if(root->left == NULL && root->right != NULL){
            BSTNode *right = root->right;
            free(root);
            return right;
        }
    }
	return root;

}



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
