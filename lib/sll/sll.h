#ifndef SLL_H
#define SLL_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct node {
   char main_major[5];
   char main_minor[5];
   char main_mp [5]; 
   struct node *pNextNode;
} node_t;

size_t sizeSLL(const node_t *pHead);

/*! Adds data in a new node at the end of the SLL. */
void addSLL(node_t **ppHead, char *major, char *minor, char *mp);

void clearSLL(node_t **ppHead);

void showSLL(const node_t *pHead);

bool compareSLL(node_t *currentNode, char *major, char *minor, char *rssi);

#endif
