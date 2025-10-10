#include "linked_list.h"
// #include "queue.h"
#define MIN_INT -1000

typedef struct _stack
{
	LinkedList ll;
}Stack;  // You should not change the definition of Stack


void createStackFromLinkedList(LinkedList *ll , Stack *stack);
void removeEvenValues(Stack *s);
int isStackPairwiseConsecutive(Stack *s);


void push(Stack *s , int item);
int pop(Stack *s);
int isEmptyStack(Stack *s);
void removeAllItemsFromStack(Stack *s);
int peek(Stack *s);
