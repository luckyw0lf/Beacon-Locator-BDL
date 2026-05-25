#include "sll.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

size_t sizeSLL(const node_t *pHead)
{
    const node_t *pNext = NULL;
    pNext = pHead;
    size_t size = 0;

    while (pNext != NULL)
    {
        size++;
        pNext = pNext->pNextNode;
    }
    return size;
}

void addSLL(node_t **ppHead, char *major, char *minor, char *mp)
{
    node_t *pNext = *ppHead;
    node_t *pNew = (node_t *)malloc(sizeof(node_t));

    /* Check if allocation has succeeded */
    if (pNew != NULL)
    {
        strcpy(pNew->main_major, major);
        strcpy(pNew->main_minor, minor);
        strcpy(pNew->main_mp, mp);
        pNew->pNextNode = NULL;
    }

    if (pNext != NULL)
    {
        /* Traverse through every subsequent node in the SLL */
        while (pNext->pNextNode != NULL)
        {
            pNext = pNext->pNextNode;
        }
        pNext->pNextNode = pNew;
    }
    else
    {
        *ppHead = pNew;
    }
}

bool compareSLL(node_t *currentNode, char *major, char *minor, char *rssi)
{
    if (currentNode == NULL)
    {
        return false;
    }
    else
    {
        if(strcmp(major, currentNode -> main_major) == 0 && strcmp (minor, currentNode -> main_minor) == 0)
        {
            int current_rssi = atoi(rssi);
            int MP = atoi (currentNode -> main_mp);
            if (current_rssi >= MP)
            {
                return true;
            }
        }
    }
    return false;
}

// void clearSLL(node_t **ppHead)
// {
//     node_t *pToBeRemoved = *ppHead;
//     node_t *pNext = NULL;

//     if (pToBeRemoved != NULL)
//     {
//         pNext = pToBeRemoved->pNextNode;
//         free(pToBeRemoved);
//         pToBeRemoved = pNext;
//     }
//     *ppHead = NULL;
// }

// void showSLL(const node_t *pHead)
// {
//     const node_t *pNext = pHead;

//     if (pHead == NULL)
//     {
//         printf("SLL is empty\n");
//     }
//     else
//     {
//         while (pNext != NULL)
//         {
//             printf("Node %p:  Data = %d  pNext = %p\n", pNext, pNext->data,
//                    pNext->pNextNode);
//             pNext = pNext->pNextNode;
//         }
//     }
// }