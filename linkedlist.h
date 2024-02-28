#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

// Define the structure for a node
typedef struct _linkedlist_node {
    void* data;
    struct _linkedlist_node* next;
} LLNode;


// Function to create a new node
LLNode* LLCreateNode();

// Function to insert a node at the beginning of the linked list
LLNode* LLInsertAtBeginning(LLNode* head);

// Function to insert a node at the end of the linked list
LLNode* LLInsertAtEnd(LLNode* head);

// Function to free memory allocated for the linked list
void LLFreeList(LLNode* head);
#endif