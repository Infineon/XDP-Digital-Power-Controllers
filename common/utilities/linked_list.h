/* ============================================================================
** Copyright (C) 2014 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/
/**
 * @file    linked_list.h
 * @brief   API for using linked lists.
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* Data types */
#include <stddef.h>     // NULL pointer
#include <stdint.h>     // uint*_t
#include "fw_bool.h"    // boolean type

/* ============================================================================
 * Public data types
 * ========================================================================= */

/** Type of the nodes of the double linked list. */
typedef struct Linked_Node Linked_Node_t;
/* Actual type of the nodes. For insertion only the element has to be initialized */
struct Linked_Node
{
    /** Pointer to an element of arbitrary type */
    void* element;
    /** Pointer to the previous node */
    Linked_Node_t* previous;
    /** Pointer to the next node */
    Linked_Node_t* next;
};

/** Type of the double linked list */
typedef struct Linked_List
{
    /** First node of this list */
    Linked_Node_t* root;
    /** Current last node of this list */
    Linked_Node_t* last;

    /** Current node of iteration */
    Linked_Node_t* iterator;

} Linked_List_t;


/* ============================================================================
 * Function declarations
 * ========================================================================= */

/**
 * Initializes a linked list with no nodes.
 * @param list  Data instance of the list (will be modified)
 */
void linked_list_init(Linked_List_t* list);

/**
 * Adds a new node to the end of the list.
 * @param list  Data instance of the list (will be modified)
 * @param node  Node that will be added (will be modified)
 * @note No dynamic memory allocation is used. Otherwise the element would have been
 *       passed as parameter and the node would have been created internally.
 */
void linked_list_back_push(Linked_List_t* list, Linked_Node_t* node);

/**
 * Removes the node from the end of the list.
 * @param list  Data instance of the list (will be modified)
 * @return Element of the last node or NULL if the list is empty
 *         (Note that it could also be a NULL element).
 */
void* linked_list_back_pop(Linked_List_t* list);

/**
 * Removes the node containing the @p element from the list.
 * @param list      Data instance of the list
 * @param element   Element of the node which will be removed
 * @return Removed node which still contains the @p element
 */
Linked_Node_t* linked_list_remove(Linked_List_t* list, const void* element);

/**
 * Access the element of the root node.
 * Also sets the iterator to for linked_list_next_get to the root node.
 * @param list  Data instance of the list (will be modified)
 * @return Element of the root node (could also be a NULL element).
 */
void* linked_list_front_get(Linked_List_t* list);

/**
 * Forward iteration: Access the element of the next node when this
 * function was called the last time.
 * @param list  Data instance of the list (will be modified)
 * @return Element of the next node or NULL when the last node was reached before.
 * @note In case nodes are removed between calls to this function the behaviour is undefined.
 */
void* linked_list_next_get(Linked_List_t* list);

/**
 * Access the element of the last node.
 * Also sets the iterator to for linked_list_previous_get to the last node.
 * @param list  Data instance of the list (will be modified)
 * @return Element of the last node or NULL if the list is empty
 *         (Note that it could also be a NULL element).
 */
void* linked_list_back_get(Linked_List_t* list);

/**
 * Backward iteration: Access the element of the previous node when this
 * function was called the last time.
 * @param list  Data instance of the list (will be modified)
 * @return Element of the previous node or NULL when the root was reached before.
 * @note In case nodes are removed between calls to this function the behaviour is undefined.
 */
void* linked_list_previous_get(Linked_List_t* list);

#endif /* LINKED_LIST_H */
