/*
 * This is a very simple linear memory allocator for
 * aligned offsets
 *
 * Victor Oliveira (victormatheus@gmail.com)
 */

/* Header structure for linear buffer. */
typedef struct {
    void       *mem;        /* Pointer to buffer memory. */
    uintptr_t   total_size; /* Total size in bytes. */
    uintptr_t   offset;     /* Offset. */
} linear_buffer;

void  linear_buffer_create  (linear_buffer **buf, int elem_n, size_t elem_size, size_t align);
void  linear_buffer_destroy (linear_buffer **buf);
void *linear_buffer_alloc   (linear_buffer *buf, size_t align, uintptr_t size);

float *linear_buffer_alloc_float (int n);

float *linear_buffer_alloc_float (linear_buffer *buf, int n);
