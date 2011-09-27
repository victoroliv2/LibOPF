#ifndef _REALHEAP_H_
#define _REALHEAP_H_

#include "common.h"

/* Auxiliary for real_heap */
#define HEAP_DAD(i)      ((i - 1) / 2)
#define HEAP_LEFTSON(i)  (2 * i + 1)
#define HEAP_RIGHTSON(i) (2 * i + 2)

enum COLOR
{
  COLOR_WHITE = 0,
  COLOR_GRAY  = 1,
  COLOR_BLACK = 2
};


enum REMOVAL_POLICY
{
  REMOVAL_POLICY_MIN = 0,
  REMOVAL_POLICY_MAX = 1
};

struct real_heap
{
  double *cost;
  enum COLOR *color;
  int   *pixel;
  int   *pos;
  int    last;
  int    n;
  enum REMOVAL_POLICY removal_policy;
};

struct     real_heap *real_heap_create  (int n, double *cost);
void       real_heap_set_removal_policy (struct real_heap  * h, enum REMOVAL_POLICY policy);
int        real_heap_is_full            (struct real_heap  * h);
int        real_heap_is_empty           (struct real_heap  * h);
void       real_heap_destroy            (struct real_heap ** h);
int        real_heap_insert             (struct real_heap  * h, int  pixel);
int        real_heap_remove             (struct real_heap  * h, int *pixel);
void       real_heap_update             (struct real_heap  * h, int p, double value);
void       real_heap_go_up              (struct real_heap  * h, int i);
void       real_heap_go_down            (struct real_heap  * h, int i);
void       real_heap_reset              (struct real_heap  * h);

#endif
