#include "stack.h"
#include "linked_list.h"
#include "queue.h"
#include <limits.h>
#include <stdlib.h> // for abs

void createStackFromLinkedList(LinkedList *ll, Stack *s) {
  if (s == NULL)
    return;

  removeAllItemsFromStack(s);

  if (ll == NULL || ll->head == NULL) {
    return;
  }

  ListNode *cur = ll->head;

  while (cur != NULL) {
    push(s, cur->item);
    cur = cur->next;
  }

  return;
}

void removeEvenValues(Stack *s) {
  // if (s == NULL) return;

  // int size = s->ll.size;
  // Stack temp = {0};

  // for (int i = 0; i < size; i++) {
  //     int poped = pop(s);
  //     // ODD
  //     if(poped%2 != 0){
  //         push(s, poped);
  //     }
  // }

  // printList(&s->ll);
  //
  if (s == NULL)
    return;

  Stack tmp; // 보조 스택
  tmp.ll.head = NULL;
  tmp.ll.size = 0;

  // 1) s에서 모두 꺼내며 홀수만 tmp로 (이 과정에서 홀수 순서는 뒤집힘)
  while (!isEmptyStack(s)) {
    int x = pop(s);
    if (x % 2 != 0)
      push(&tmp, x);
  }

  // 2) tmp -> s로 다시 옮기기 (한 번 더 뒤집혀서 원래 순서로 복구)
  while (!isEmptyStack(&tmp)) {
    push(s, pop(&tmp));
  }
}

int isStackPairwiseConsecutive(Stack *s) {
  if (s == NULL || s->ll.size % 2 == 1)
    return 0;

  Stack tmp; // 보조 스택
  tmp.ll.head = NULL;
  tmp.ll.size = 0;

  while (!isEmptyStack(s)) {
    int x = pop(s);
    int y = pop(s);
    if (abs(x - y) != 1) {
      return 0;
    }
    push(&tmp, x);
    push(&tmp, y);
  }
  while (!isEmptyStack(&tmp)) {
    push(s, pop(&tmp));
  }

  printList(&s->ll);
  return 1;
}

// use stack, push, pop to implement reverse queue
void reverse(Queue *q) {
  Stack s; // 보조 스택
  s.ll.head = NULL;
  s.ll.size = 0;

  while (isEmptyQueue(q) == 0) {
    push(&s, dequeue(q));
  }

  while (isEmptyStack(&s) == 0) {
    enqueue(q, pop(&s));
  }
  printList(&q->ll);
}

void removeUntilStack(Stack *s, int value) {
  if (s == NULL) {
    return;
  }
  while (isEmptyStack(s) == 0) {
    int item = pop(s);
    if (item == value) {
      push(s, item);
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////

void push(Stack *s, int item) { insertNode(&(s->ll), 0, item); }

int pop(Stack *s) {
  int item;
  if (s->ll.head != NULL) {
    item = ((s->ll).head)->item;
    removeNode(&(s->ll), 0);
    return item;
  } else
    return MIN_INT;
}

int peek(Stack *s){
   return ((s->ll).head)->item;
}

int isEmptyStack(Stack *s) {
  if ((s->ll).size == 0)
    return 1;
  else
    return 0;
}

void removeAllItemsFromStack(Stack *s) {
  if (s == NULL)
    return;
  while (s->ll.head != NULL) {
    pop(s);
  }
}

char getClosingBracket(char open) {
    switch (open) {
        case '(': return ')';
        case '{': return '}';
        case '[': return ']';
        default:  return '\0'; // invalid input
    }
}


int balanced(char *expression) {
  Stack s = {};
  s.ll.size = 0;
  s.ll.head = NULL;
  int i = 0;
  while (expression[i] != '\0') {
    if (expression[i] == '(' || expression[i] == '{' || expression[i] == '[') {
      push(&s, expression[i]);
    } else {
        if (isEmptyStack(&s)) return 0;
        if(getClosingBracket(peek(&s)) == expression[i]){
            pop(&s);
        }
        else{
            return 0;
        }
    }
    i++;
  }
  if (isEmptyStack(&s)){
      return 1;
  }
  // not balanced
  return 0;
}
