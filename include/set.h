#ifndef _SET_H_
#define _SET_H_

#include "common.h"

typedef struct _set
{
  int elem;
  struct _set *next;
} set;

void  set_insert   (set **s, int elem);
int   set_remove   (set **s);
int   set_get_size (set  *s);
set  *set_clone    (set  *s);
void  set_destroy  (set **s);

#endif
