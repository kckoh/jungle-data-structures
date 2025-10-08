#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

typedef struct _listnode
{
    int item;
    struct _listnode *next;
} ListNode;

typedef struct _linkedlist
{
    int size;
    ListNode *head;
} LinkedList;

// 함수 프로토타입
int insertSortedLL(LinkedList *ll, int item);
void alternateMergeLinkedList(LinkedList *ll1, LinkedList *ll2); 
void moveOddItemsToBackLL(LinkedList *ll);
void moveEvenItemsToBackLL(LinkedList *ll);
void frontBackSplitLL(LinkedList *ll, LinkedList
*resultFrontList, LinkedList *resultBackList);
int moveMaxToFront(ListNode **ptrHead);
void recursiveReverse(ListNode **ptrHead);




void printList(LinkedList *ll);
void removeAllItems(LinkedList *ll);
ListNode *findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);
int getItemAt(LinkedList *ll, int index);
int* returnList(LinkedList *ll, int size);

#endif
