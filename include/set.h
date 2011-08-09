#ifndef _SET_H_
#define _SET_H_

#include "common.h"

struct set
{
  int elem;
  struct set *next;
};

void        set_insert   (struct set **s, int elem);
int         set_remove   (struct set **s);
int         set_get_size (struct set  *s);
struct set *set_clone    (struct set  *s);
void        set_destroy  (struct set **s);

#endif
