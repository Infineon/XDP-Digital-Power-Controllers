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
 * @file    circular_buffer.h
 * @brief   API for using circular buffers.
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

/* Data types */
#include <stddef.h>     // NULL pointer
#include <stdint.h>     // uint*_t
#include "fw_bool.h"    // boolean type


/* ============================================================================
 * Public data types
 * ========================================================================= */

/** Elementary data type of the elements of a circular buffer */
typedef void* Circ_Buffer_Element_t;

/** Type of circular buffers */
typedef struct
{
    /** Variable-sized array of elements */
    Circ_Buffer_Element_t* elements;
    /** Maximum number of elements */
    uint32_t size;
    /** Current number of elements */
    uint32_t count;

    /** Index at which to write new element  */
    uint32_t i_write;
    /** Index at which to read oldest element */
    uint32_t i_read;

} Circ_Buffer_t;


/* ============================================================================
 * Function declarations
 * ========================================================================= */

/**
 * Initializes the circular buffer.
 * @param circ_buffer      Data instance of the circular buffer (will be modified)
 * @param size      Number of elements for the buffer
 * @param memory    Memory space for the elements of the buffer
 */
void circ_buffer_init(Circ_Buffer_t* circ_buffer, uint32_t size, Circ_Buffer_Element_t memory[]);

/**
 * Stores a new element a the end of the buffer. If the buffer is full
 * the oldest element will be overwritten.
 * @param circ_buffer      Data instance of the circular buffer (will be modified)
 * @param element   New element
 */
void circ_buffer_push(Circ_Buffer_t* circ_buffer, Circ_Buffer_Element_t element);

/**
 * Retrieves the oldest element from the buffer.
 * @param circ_buffer   Data instance of the circular buffer (will be modified)
 * @return the oldest element or NULL if the buffer was empty.
 */
Circ_Buffer_Element_t circ_buffer_pop(Circ_Buffer_t* circ_buffer);

/**
 * Indicates if the circular buffer is full.
 * @param circ_buffer   Data instance of the circular buffer
 * @return boolean_true if the buffer is full, otherwise boolean_false.
 */
boolean circ_buffer_is_full(const Circ_Buffer_t* circ_buffer);

/**
 * Indicates if the circular buffer is empty.
 * @param circ_buffer   Data instance of the circular buffer
 * @return boolean_true if the buffer is empty, otherwise boolean_false.
 */
boolean circ_buffer_is_empty(const Circ_Buffer_t* circ_buffer);

/**
 * Efficiently removes @p num_elements from the circular buffer by just
 * advancing the read index instead of popping elements one by one with
 * circ_buffer_pop.
 * @param circ_buffer   Data instance of the circular buffer
 * @param num_elements  Number of elements to be removed
 */
void circ_buffer_elements_remove(Circ_Buffer_t* circ_buffer, uint32_t num_elements);

#endif /* CIRCULAR_BUFFER_H */

