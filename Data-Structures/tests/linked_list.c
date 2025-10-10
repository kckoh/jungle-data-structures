#include "linked_list.h"
#include <stdio.h>

int insertSortedLL(LinkedList *ll, int item) {
  // cases: NULL -> return -1
  // insert into empty list
  // insert when there are more than one items
  // new comment
  if (ll == NULL)
    return -1;
  int idx = 0;
  ListNode *cur;
  cur = ll->head;
  // insert into empty list
  if (ll->head == NULL) {
    return insertNode(ll, idx, item);
  }

  while (cur != NULL && cur->item < item) {
    idx++;
    cur = cur->next;
  }

  if (cur != NULL && cur->item == item)
    return -1;

  // insert at the end
  int status = insertNode(ll, idx, item);
  if (status == -1)
    return status;
  printList(ll);
  return idx;
}

// insert the second linkedlist from the first linkedlist
//
void alternateMergeLinkedList(LinkedList *ll1, LinkedList *ll2) {

  // 0 insert 1 insert 2 insert 3
  //
  if (ll1 == NULL || ll2 == NULL) {
    return;
  }

  ListNode *first = ll1->head;
  ListNode *second = ll2->head;

  if (first == NULL || second == NULL) {
    return;
  }
  int idx = 1;
  // approach:
  while (first != NULL && second != NULL) {
    insertNode(ll1, idx, second->item);
    first = first->next;
    first = first ? first->next : NULL;
    second = second->next;
    removeNode(ll2, 0);
    idx = idx + 2;
  }
}

void moveOddItemsToBackLL(LinkedList *ll) {
  // keep track of the index
  // get the odd ListNodes and append it at the end
  if (ll == NULL || ll->head == NULL || ll->head->next == NULL) {
    return;
  }
  ListNode *cur = ll->head;
  ListNode *tail = ll->head;
  ListNode *prev = NULL;
  // find the tail first
  int count = 1;
  while (tail->next != NULL) {
    tail = tail->next;
    count++;
  }

  while (count > 0) {
    ListNode *next = cur->next;
    // ODD
    if (cur->item % 2 != 0) {
      // head = ODD
      if (prev == NULL) {
        ll->head = next;
      } else {
        prev->next = next;
      }

      // tail에 추가
      tail->next = cur;
      cur->next = NULL;
      tail = tail->next;
      cur = next;
    }
    // EVEN
    else {
      prev = cur;
      cur = next;
    }
    count--;
  }
}

void moveEvenItemsToBackLL(LinkedList *ll) {
  // keep track of the index
  // get the odd ListNodes and append it at the end
  if (ll == NULL || ll->head == NULL || ll->head->next == NULL) {
    return;
  }
  ListNode *cur = ll->head;
  ListNode *tail = ll->head;
  ListNode *prev = NULL;
  // find the tail first
  int count = 1;
  while (tail->next != NULL) {
    tail = tail->next;
    count++;
  }

  while (count > 0) {
    ListNode *next = cur->next;
    // ODD
    if (cur->item % 2 == 0) {
      // head = ODD
      if (prev == NULL) {
        ll->head = next;
      } else {
        prev->next = next;
      }

      // tail에 추가
      tail->next = cur;
      cur->next = NULL;
      tail = tail->next;
      cur = next;
    }
    // EVEN
    else {
      prev = cur;
      cur = next;
    }
    count--;
  }
}

void frontBackSplitLL(LinkedList *ll, LinkedList *resultFrontList,
                      LinkedList *resultBackList) {
  // get the size
  // even -> size//2
  // odd -> size//2 + 1
  // while

  if (ll == NULL) {
    return;
  }

  ListNode *cur = ll->head;

  int idx = 0;
  int front_idx = 0;
  int back_idx = 0;
  int half = (ll->size % 2 == 0) ? (ll->size / 2) : (ll->size / 2 + 1);

  while (cur != NULL) {
    // add it to the front
    if (idx < half) {
      insertNode(resultFrontList, front_idx, cur->item);
      front_idx++;
    }
    // add it to the end
    else {
      insertNode(resultBackList, back_idx, cur->item);
      back_idx++;
    }
    idx++;
    cur = cur->next;
  }
  printList(resultFrontList);
  printList(resultBackList);
}

// 1 hour
int moveMaxToFront(ListNode **ptrHead) {
  if (ptrHead == NULL || *ptrHead == NULL) {
    return 0;
  }

  ListNode *cur = *ptrHead;
  ListNode *prev = NULL;
  int max = cur->item;
  ListNode *maxNode = cur;
  ListNode *maxPrev = NULL;

  while (cur != NULL) {
    if (cur->item > max) {
      max = cur->item;
      maxNode = cur;
      maxPrev = prev;
    }
    prev = cur;
    cur = cur->next;
  }

  // maxNode is at the beginning
  if (maxPrev == NULL) {
    return 1;
  }
  // maxNode is at the end
  maxPrev->next = maxNode->next;
  maxNode->next = *ptrHead;
  *ptrHead = maxNode;

  return 1;
}

///////////////////////////////////////////////////////////////////////////////////

void printList(LinkedList *ll) {

  ListNode *cur;
  if (ll == NULL)
    return;
  cur = ll->head;

  if (cur == NULL)
    printf("Empty");
  while (cur != NULL) {
    printf("%d ", cur->item);
    cur = cur->next;
  }
  printf("\n");
}

int *returnList(LinkedList *ll, int size) {
  int *arr = malloc(size * sizeof(int));
  int idx = 0;
  ListNode *cur;
  if (ll == NULL)
    return NULL;
  cur = ll->head;

  if (cur == NULL)
    return NULL;

  while (cur != NULL) {
    arr[idx++] = cur->item;
    cur = cur->next;
  }
  return arr;
}

void removeAllItems(LinkedList *ll) {
  ListNode *cur = ll->head;
  ListNode *tmp;

  while (cur != NULL) {
    tmp = cur->next;
    free(cur);
    cur = tmp;
  }
  ll->head = NULL;
  ll->size = 0;
}

ListNode *findNode(LinkedList *ll, int index) {

  ListNode *temp;

  if (ll == NULL || index < 0 || index >= ll->size)
    return NULL;

  temp = ll->head;

  if (temp == NULL || index < 0)
    return NULL;

  while (index > 0) {
    temp = temp->next;
    if (temp == NULL)
      return NULL;
    index--;
  }

  return temp;
}

int insertNode(LinkedList *ll, int index, int value) {

  ListNode *pre, *cur;

  if (ll == NULL || index < 0 || index > ll->size + 1)
    return -1;

  // If empty list or inserting first node, need to update head pointer
  if (ll->head == NULL || index == 0) {
    cur = ll->head;
    ll->head = malloc(sizeof(ListNode));
    ll->head->item = value;
    ll->head->next = cur;
    ll->size++;
    return 0;
  }

  // Find the nodes before and at the target position
  // Create a new node and reconnect the links
  if ((pre = findNode(ll, index - 1)) != NULL) {
    cur = pre->next;
    pre->next = malloc(sizeof(ListNode));
    pre->next->item = value;
    pre->next->next = cur;
    ll->size++;
    return 0;
  }

  return -1;
}

int removeNode(LinkedList *ll, int index) {

  ListNode *pre, *cur;

  // Highest index we can remove is size-1
  if (ll == NULL || index < 0 || index >= ll->size)
    return -1;

  // If removing first node, need to update head pointer
  if (index == 0) {
    cur = ll->head->next;
    free(ll->head);
    ll->head = cur;
    ll->size--;

    return 0;
  }

  // Find the nodes before and after the target position
  // Free the target node and reconnect the links
  if ((pre = findNode(ll, index - 1)) != NULL) {

    if (pre->next == NULL)
      return -1;

    cur = pre->next;
    pre->next = cur->next;
    free(cur);
    ll->size--;
    return 0;
  }

  return -1;
}

int getItemAt(LinkedList *ll, int index) {
  ListNode *node = findNode(ll, index);
  return node ? node->item : -1;
}
