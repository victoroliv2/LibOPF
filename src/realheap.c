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

#include "realheap.h"

void
SetRemovalPolicyRealHeap (RealHeap * H, char policy)
{
  if (H->removal_policy != policy)
    {
      H->removal_policy = policy;
      ResetRealHeap (H);
    }
}

void
GoUpRealHeap (RealHeap * H, int i)
{
  int j = HEAP_DAD (i);

  if (H->removal_policy == MINVALUE)
    {

      while ((i > 0) && (H->cost[H->pixel[j]] > H->cost[H->pixel[i]]))
        {
          Change (&H->pixel[j], &H->pixel[i]);
          H->pos[H->pixel[i]] = i;
          H->pos[H->pixel[j]] = j;
          i = j;
          j = HEAP_DAD (i);
        }
    }
  else                          /* removal_policy == MAXVALUE */
    {

      while ((i > 0) && (H->cost[H->pixel[j]] < H->cost[H->pixel[i]]))
        {
          Change (&H->pixel[j], &H->pixel[i]);
          H->pos[H->pixel[i]] = i;
          H->pos[H->pixel[j]] = j;
          i = j;
          j = HEAP_DAD (i);
        }
    }
}

void
GoDownRealHeap (RealHeap * H, int i)
{
  int j, left = HEAP_LEFTSON (i), right = HEAP_RIGHTSON (i);

  j = i;
  if (H->removal_policy == MINVALUE)
    {

      if ((left <= H->last) &&
          (H->cost[H->pixel[left]] < H->cost[H->pixel[i]]))
        j = left;
      if ((right <= H->last) &&
          (H->cost[H->pixel[right]] < H->cost[H->pixel[j]]))
        j = right;
    }
  else                          /* removal_policy == MAXVALUE */
    {

      if ((left <= H->last) &&
          (H->cost[H->pixel[left]] > H->cost[H->pixel[i]]))
        j = left;
      if ((right <= H->last) &&
          (H->cost[H->pixel[right]] > H->cost[H->pixel[j]]))
        j = right;
    }

  if (j != i)
    {
      Change (&H->pixel[j], &H->pixel[i]);
      H->pos[H->pixel[i]] = i;
      H->pos[H->pixel[j]] = j;
      GoDownRealHeap (H, j);
    }
}

char
IsFullRealHeap (RealHeap * H)
{
  if (H->last == (H->n - 1))
    return 1;
  else
    return 0;
}

char
IsEmptyRealHeap (RealHeap * H)
{
  if (H->last == -1)
    return 1;
  else
    return 0;
}

RealHeap *
CreateRealHeap (int n, float *cost)
{
  RealHeap *H = NULL;
  int i;

  if (cost == NULL)
    {
      fprintf (stdout,
               "Cannot create heap without cost map in CreateRealHeap");
      return NULL;
    }

  H = (RealHeap *) malloc (sizeof (RealHeap));
  if (H != NULL)
    {
      H->n = n;
      H->cost = cost;
      H->color = (char *) malloc (sizeof (char) * n);
      H->pixel = (int *) malloc (sizeof (int) * n);
      H->pos = (int *) malloc (sizeof (int) * n);
      H->last = -1;
      H->removal_policy = MINVALUE;
      if (H->color == NULL || H->pos == NULL || H->pixel == NULL)
        Error (MSG1, "CreateRealHeap");
      for (i = 0; i < H->n; i++)
        {
          H->color[i] = WHITE;
          H->pos[i] = -1;
          H->pixel[i] = -1;
        }
    }
  else
    Error (MSG1, "CreateRealHeap");

  return H;
}

void
DestroyRealHeap (RealHeap ** H)
{
  RealHeap *aux = *H;
  if (aux != NULL)
    {
      if (aux->pixel != NULL)
        free (aux->pixel);
      if (aux->color != NULL)
        free (aux->color);
      if (aux->pos != NULL)
        free (aux->pos);
      free (aux);
      *H = NULL;
    }
}

char
InsertRealHeap (RealHeap * H, int pixel)
{
  if (!IsFullRealHeap (H))
    {
      H->last++;
      H->pixel[H->last] = pixel;
      H->color[pixel] = GRAY;
      H->pos[pixel] = H->last;
      GoUpRealHeap (H, H->last);
      return 1;
    }
  else
    return 0;
}

char
RemoveRealHeap (RealHeap * H, int *pixel)
{
  if (!IsEmptyRealHeap (H))
    {
      *pixel = H->pixel[0];
      H->pos[*pixel] = -1;
      H->color[*pixel] = BLACK;
      H->pixel[0] = H->pixel[H->last];
      H->pos[H->pixel[0]] = 0;
      H->pixel[H->last] = -1;
      H->last--;
      GoDownRealHeap (H, 0);
      return 1;
    }
  else
    return 0;
}


void
UpdateRealHeap (RealHeap * H, int p, float value)
{
  H->cost[p] = value;

  if (H->color[p] == BLACK)
    {
      printf ("error: p has been removed\n");
    }

  if (H->color[p] == WHITE)
    InsertRealHeap (H, p);
  else
    GoUpRealHeap (H, H->pos[p]);
}

void
ResetRealHeap (RealHeap * H)
{
  int i;

  for (i = 0; i < H->n; i++)
    {
      H->color[i] = WHITE;
      H->pos[i] = -1;
      H->pixel[i] = -1;
    }
  H->last = -1;
}
