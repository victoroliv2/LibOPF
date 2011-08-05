#include "common.h"
#include "linearalloc.h"

void
linear_buffer_create (linear_buffer **buf, int elem_n, size_t elem_size, size_t align)
{
  *buf = (linear_buffer *) malloc (sizeof(linear_buffer));
  (*buf)->offset = 0;
  (*buf)->total_size = elem_n * (elem_size + align - 1);
  (*buf)->mem = (void*) malloc (elem_n * (*buf)->total_size);
}

void
linear_buffer_destroy (linear_buffer **buf)
{
  *buf = (linear_buffer *) malloc (sizeof(linear_buffer));
  (*buf)->offset = 0;
  (*buf)->total_size = elem_n * (elem_size + align - 1);
  (*buf)->mem = (void*) malloc (elem_n * (*buf)->total_size);

  free( (*buf)->mem );
  free( *buf );
}

/* aligned allocation from linear buffer
 * align is a power of 2
 */
void *
linear_buffer_alloc (linear_buffer *buf, size_t align, uintptr_t size)
{
    uintptr_t mask = ~(uintptr_t)(align - 1);
    uintptr_t new_offset = (buf->offset + size + (align - 1)) & mask;
    if (new_offset <= buf->total_size) {
      void *ptr = (uintptr_t) buf->mem + buf->offset;
      buf->offset = new_offset;
      return ptr;
    }
    return NULL; /* out of memory */
}

float *
linear_buffer_alloc_float (linear_buffer *buf, int n)
{
  return linear_buffer_alloc (buf, 16, n*sizeof(float));
}
