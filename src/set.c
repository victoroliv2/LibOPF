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
set_insert (struct set ** s, int elem)
{
  struct set *p = NULL;

  p = (struct set *) calloc (1, sizeof (struct set));
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
set_remove (struct set ** s)
{
  struct set *p;
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
set_get_size (struct set * s)
{
  struct set *aux;
  int size = 0;

  aux = s;
  while (aux != NULL)
    {
      size++;
      aux = aux->next;
    }
  return size;
}

struct set *
set_clone (struct set * s)
{
  struct set *tmp = NULL;
  struct set *C = NULL;
  struct set **tail = NULL;
  int p;

  tmp = s;

  if (tmp != NULL)
    {
      p = tmp->elem;
      C = (struct set *) calloc (1, sizeof (struct set));
      C->elem = p;
      C->next = NULL;
      tail = &(C->next);
      tmp = tmp->next;
    }

  while (tmp != NULL)
    {
      p = tmp->elem;
      *tail = (struct set *) calloc (1, sizeof (struct set));
      (*tail)->elem = p;
      (*tail)->next = NULL;
      tail = &((*tail)->next);
      tmp = tmp->next;
    }
  return C;
}

void
set_destroy (struct set ** s)
{
  struct set *p;
  while (*s != NULL)
    {
      p = *s;
      *s = p->next;
      free (p);
    }
}
