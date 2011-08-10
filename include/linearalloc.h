/*
 * This is a very simple linear memory allocator for
 * aligned offsets
 *
 * Victor Oliveira (victormatheus@gmail.com)
 */

#ifndef __LINEAR_ALLOC_H__
#define __LINEAR_ALLOC_H__

#include <stdint.h>
#include <stdlib.h>

struct linear_buffer{
    void       *mem;        /* Pointer to buffer memory. */
    uintptr_t   total_size; /* Total size in bytes. */
    uintptr_t   align;
    uintptr_t   offset;     /* Offset. */
};

void  linear_buffer_create  (struct linear_buffer **buf, int elem_n, size_t elem_size, size_t align);
void  linear_buffer_destroy (struct linear_buffer **buf);
void *linear_buffer_alloc   (struct linear_buffer *buf, uintptr_t size);

float *linear_buffer_alloc_float (struct linear_buffer *buf, int n);

#endif
