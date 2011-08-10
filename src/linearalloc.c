#include "common.h"
#include "linearalloc.h"

void
linear_buffer_create (struct linear_buffer **buf, int elem_n, size_t elem_size, size_t align)
{
  *buf = (struct linear_buffer *) malloc (sizeof(struct linear_buffer));
  (*buf)->offset = 0;
  (*buf)->align = align;
  (*buf)->total_size = elem_n * elem_size + align - 1;
  (*buf)->mem = (void*) malloc (elem_n * (*buf)->total_size);
}

void
linear_buffer_destroy (struct linear_buffer **buf)
{
  free( (*buf)->mem );
  free( *buf );
}

/* aligned allocation from linear buffer
 * align is a power of 2
 */
void *
linear_buffer_alloc (struct linear_buffer *buf, uintptr_t size)
{
    uintptr_t mask = ~(uintptr_t)(buf->align - 1);
    uintptr_t new_offset = buf->offset + ((size + (buf->align - 1)) & mask);
    if (new_offset <= buf->total_size) {
      void *ptr = (void*) ((uintptr_t) buf->mem + buf->offset);
      buf->offset = new_offset;
      return ptr;
    }
    return NULL; /* out of memory */
}

float *
linear_buffer_alloc_float (struct linear_buffer *buf, int n)
{
  return linear_buffer_alloc (buf, n*sizeof(float));
}
