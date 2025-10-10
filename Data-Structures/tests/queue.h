#include "linked_list.h"

typedef struct _queue
{
	LinkedList ll;
} Queue;  // You should not change the definition of Queue

void createQueueFromLinkedList(LinkedList *ll, Queue *q);
void removeOddValues(Queue *q);
void reverse(Queue *q);
void recursiveReverseQueue(Queue *q);

void enqueue(Queue *q, int item);
int dequeue(Queue *q);
int isEmptyQueue(Queue *q);
void removeAllItemsFromQueue(Queue *q);
