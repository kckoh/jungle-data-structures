#include "linked_list.h"
#include <limits.h>
#include "stack.h"

void createStackFromLinkedList(LinkedList *ll, Stack *s)
{
    if (s == NULL) return;

    removeAllItemsFromStack(s);

    if(ll == NULL || ll->head ==NULL){
        return;
    }

    ListNode *cur = ll->head;

    while(cur != NULL){
        push(s,cur->item);
        cur = cur->next;
    }

    return;

}

void removeEvenValues(Stack *s)
{
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
      if (s == NULL) return;

        Stack tmp;              // 보조 스택
        tmp.ll.head = NULL;
        tmp.ll.size = 0;

        // 1) s에서 모두 꺼내며 홀수만 tmp로 (이 과정에서 홀수 순서는 뒤집힘)
        while (!isEmptyStack(s)) {
            int x = pop(s);
            if (x % 2 != 0) push(&tmp, x);
        }

        // 2) tmp -> s로 다시 옮기기 (한 번 더 뒤집혀서 원래 순서로 복구)
        while (!isEmptyStack(&tmp)) {
            push(s, pop(&tmp));
        }

}

//////////////////////////////////////////////////////////////////////////////////

void push(Stack *s, int item)
{
	insertNode(&(s->ll), 0, item);
}

int pop(Stack *s)
{
	int item;
	if (s->ll.head != NULL)
	{
		item = ((s->ll).head)->item;
		removeNode(&(s->ll), 0);
		return item;
	}
	else
		return MIN_INT;
}

int isEmptyStack(Stack *s)
{
	if ((s->ll).size == 0)
		return 1;
	else
		return 0;
}


void removeAllItemsFromStack(Stack *s)
{
	if (s == NULL)
		return;
	while (s->ll.head != NULL)
	{
		pop(s);
	}
}
