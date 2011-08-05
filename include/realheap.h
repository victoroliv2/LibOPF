#ifndef _REALHEAP_H_
#define _REALHEAP_H_

#include "common.h"

/* Auxiliary for real_heap */
#define HEAP_DAD(i)      ((i - 1) / 2)
#define HEAP_LEFTSON(i)  (2 * i + 1)
#define HEAP_RIGHTSON(i) (2 * i + 2)

typedef enum
{
  COLOR_WHITE = 0,
  COLOR_GRAY  = 1,
  COLOR_BLACK = 2
} COLOR;


typedef enum
{
  REMOVAL_POLICY_MIN = 0,
  REMOVAL_POLICY_MAX = 1
} REMOVAL_POLICY;

typedef struct
{
  float *cost;
  COLOR *color;
  int   *pixel;
  int   *pos;
  int    last;
  int    n;
  REMOVAL_POLICY removal_policy;
} real_heap;

real_heap *real_heap_create             (int n, float *cost);
void       real_heap_set_removal_policy (real_heap  * h, REMOVAL_POLICY policy);
int        real_heap_is_full            (real_heap  * h);
int        real_heap_is_empty           (real_heap  * h);
void       real_heap_destroy            (real_heap ** h);
int        real_heap_insert             (real_heap  * h, int  pixel);
int        real_heap_remove             (real_heap  * h, int *pixel);
void       real_heap_update             (real_heap  * h, int p, float value);
void       real_heap_go_up              (real_heap  * h, int i);
void       real_heap_go_down            (real_heap  * h, int i);
void       real_heap_reset              (real_heap  * h);

#endif
