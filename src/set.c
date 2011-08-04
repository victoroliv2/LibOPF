/*
  Copyright (C) <2009-2011> <Alexandre Xavier Falcão and João Paulo Papa>

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
  Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  please see full copyright in COPYING file.

  -------------------------------------------------------------------------
  written by A.X. Falcão <afalcao@ic.unicamp.br> and by J.P. Papa
  <papa.joaopaulo@gmail.com>, Oct 20th 2008

  This program is a collection of functions to manage the Optimum-Path Forest (OPF)
  classifier.*/

#include "set.h"

void
set_insert (set ** s, int elem)
{
  set *p = NULL;

  p = (set *) calloc (1, sizeof (set));
  if (p == NULL)
    error (LOG_OUT_OF_MEMORY);
  if (*s == NULL)
    {
      p->elem = elem;
      p->next = NULL;
    }
  else
    {
      p->elem = elem;
      p->next = *s;
    }
  *s = p;
}

int
set_remove (set ** s)
{
  set *p;
  int elem = NIL;

  if (*s != NULL)
    {
      p = *s;
      elem = p->elem;
      *s = p->next;
      free (p);
    }

  return elem;
}


int
set_get_size (set * s)
{
  set *aux;
  int size = 0;

  aux = s;
  while (aux != NULL)
    {
      size++;
      aux = aux->next;
    }
  return size;
}

set *
set_clone (set * s)
{
  set *tmp = NULL;
  set *C = NULL;
  set **tail = NULL;
  int p;

  tmp = s;

  if (tmp != NULL)
    {
      p = tmp->elem;
      C = (set *) calloc (1, sizeof (set));
      C->elem = p;
      C->next = NULL;
      tail = &(C->next);
      tmp = tmp->next;
    }

  while (tmp != NULL)
    {
      p = tmp->elem;
      *tail = (set *) calloc (1, sizeof (set));
      (*tail)->elem = p;
      (*tail)->next = NULL;
      tail = &((*tail)->next);
      tmp = tmp->next;
    }
  return C;
}

void
set_destroy (set ** s)
{
  set *p;
  while (*s != NULL)
    {
      p = *s;
      *s = p->next;
      free (p);
    }
}
