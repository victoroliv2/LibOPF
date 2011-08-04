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
InsertSet (Set ** S, int elem)
{
  Set *p = NULL;

  p = (Set *) calloc (1, sizeof (Set));
  if (p == NULL)
    Error (MSG1, "InsertSet");
  if (*S == NULL)
    {
      p->elem = elem;
      p->next = NULL;
    }
  else
    {
      p->elem = elem;
      p->next = *S;
    }
  *S = p;
}

int
RemoveSet (Set ** S)
{
  Set *p;
  int elem = NIL;

  if (*S != NULL)
    {
      p = *S;
      elem = p->elem;
      *S = p->next;
      //printf("RemoveSet before free");
      free (p);
      //printf(" RemoveSet after free: elem is %d\n",elem);
      //if(*S != NULL) printf(" *S->elem is %d\n",(*S)->elem);
    }

  return (elem);
}


int
GetSetSize (Set * S)
{
  Set *aux;
  int size = 0;

  aux = S;
  while (aux != NULL)
    {
      size++;
      aux = aux->next;
    }
  return size;
}

Set *
CloneSet (Set * S)
{
  Set *tmp = NULL;
  Set *C = NULL;
  Set **tail = NULL;
  int p;

  tmp = S;

  if (tmp != NULL)
    {
      p = tmp->elem;
      C = (Set *) calloc (1, sizeof (Set));
      C->elem = p;
      C->next = NULL;
      tail = &(C->next);
      tmp = tmp->next;
    }

  while (tmp != NULL)
    {
      p = tmp->elem;
      *tail = (Set *) calloc (1, sizeof (Set));
      (*tail)->elem = p;
      (*tail)->next = NULL;
      tail = &((*tail)->next);
      tmp = tmp->next;
    }
  return C;
}

void
DestroySet (Set ** S)
{
  Set *p;
  while (*S != NULL)
    {
      p = *S;
      *S = p->next;
      free (p);
    }
}
