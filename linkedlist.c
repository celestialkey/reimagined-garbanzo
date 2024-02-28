#include <stdlib.h>
#include "linkedlist.h"

// Function to create a new node
LLNode* LLCreateNode() {
    LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
    if (newNode == NULL) {
        return NULL;
    }
    newNode->data = NULL;
    newNode->next = NULL;
    return newNode;
}

// Function to insert a node at the beginning of the linked list
LLNode* LLInsertAtBeginning(LLNode* head) {
    LLNode* newNode = LLCreateNode();
    if(newNode == NULL) return NULL;
    newNode->next = head;
    return newNode;
}

// Function to insert a node at the end of the linked list
LLNode* LLInsertAtEnd(LLNode* head) {
    LLNode* newNode = LLCreateNode();
    if(newNode == NULL) return NULL;

    LLNode* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
    return newNode;
}

// Function to free memory allocated for the linked list
void LLFreeList(LLNode* head) {
    LLNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        if(temp->data != NULL)
            free(temp->data);
        free(temp);
    }
}