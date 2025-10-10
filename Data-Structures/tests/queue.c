#include "queue.h"
#include <math.h>
//////////////////////////////////////////////////////////////////////////////////

void createQueueFromLinkedList(LinkedList *ll, Queue *q) {
  /* add your code here */
  if (ll == NULL) {
    return;
  }

  ListNode *cur = ll->head;
  if (isEmptyQueue(q) == 0) {
    removeAllItemsFromQueue(q);
  }

  if (ll->head == NULL) {
    return;
  }
  while (cur != NULL) {
    enqueue(q, cur->item);
    cur = cur->next;
  }
}

void removeOddValues(Queue *q) {
  if (isEmptyQueue(q) || q->ll.head == NULL) {
    return;
  }
  ListNode *cur = q->ll.head;
  int size = q->ll.size;

  for (int i = 0; i < size; i++) {
    // EVEN
    int item = dequeue(q);
    if (item % 2 == 0) {
      enqueue(q, item);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////

void enqueue(Queue *q, int item) { insertNode(&(q->ll), q->ll.size, item); }

int dequeue(Queue *q) {
  int item;

  if (!isEmptyQueue(q)) {
    item = ((q->ll).head)->item;
    removeNode(&(q->ll), 0);
    return item;
  }
  return -1;
}

int isEmptyQueue(Queue *q) {
  if ((q->ll).size == 0)
    return 1;
  return 0;
}

void removeAllItemsFromQueue(Queue *q) {
  int count, i;
  if (q == NULL)
    return;
  count = q->ll.size;

  for (i = 0; i < count; i++)
    dequeue(q);
}
