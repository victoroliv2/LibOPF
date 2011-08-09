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
real_heap_set_removal_policy (real_heap * H, REMOVAL_POLICY policy)
{
  if (H->removal_policy != policy)
    {
      H->removal_policy = policy;
      real_heap_reset (H);
    }
}

void
real_heap_go_up (real_heap * H, int i)
{
  int j = HEAP_DAD (i);

  if (H->removal_policy == REMOVAL_POLICY_MIN)
    {

      while ((i > 0) && (H->cost[H->pixel[j]] > H->cost[H->pixel[i]]))
        {
          SWAP (&H->pixel[j], &H->pixel[i]);
          H->pos[H->pixel[i]] = i;
          H->pos[H->pixel[j]] = j;
          i = j;
          j = HEAP_DAD (i);
        }
    }
  else                          /* removal_policy == REMOVAL_POLICY_MAX */
    {

      while ((i > 0) && (H->cost[H->pixel[j]] < H->cost[H->pixel[i]]))
        {
          SWAP (&H->pixel[j], &H->pixel[i]);
          H->pos[H->pixel[i]] = i;
          H->pos[H->pixel[j]] = j;
          i = j;
          j = HEAP_DAD (i);
        }
    }
}

void
real_heap_go_down (real_heap * H, int i)
{
  int j, left = HEAP_LEFTSON (i), right = HEAP_RIGHTSON (i);

  j = i;
  if (H->removal_policy == REMOVAL_POLICY_MIN)
    {

      if ((left <= H->last) &&
          (H->cost[H->pixel[left]] < H->cost[H->pixel[i]]))
        j = left;
      if ((right <= H->last) &&
          (H->cost[H->pixel[right]] < H->cost[H->pixel[j]]))
        j = right;
    }
  else                          /* removal_policy == REMOVAL_POLICY_MAX */
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
      SWAP (&H->pixel[j], &H->pixel[i]);
      H->pos[H->pixel[i]] = i;
      H->pos[H->pixel[j]] = j;
      real_heap_go_down (H, j);
    }
}

int
real_heap_is_full (real_heap * H)
{
  if (H->last == (H->n - 1))
    return 1;
  else
    return 0;
}

int
real_heap_is_empty (real_heap * H)
{
  if (H->last == NIL)
    return 1;
  else
    return 0;
}

real_heap *
real_heap_create (int n, float *cost)
{
  real_heap *H = NULL;
  int i;

  if (cost == NULL)
    {
      fprintf (stdout,
               "Cannot create heap without cost map in create_real_heap");
      return NULL;
    }

  H = (real_heap *) malloc (sizeof (real_heap));
  if (H != NULL)
    {
      H->n = n;
      H->cost = cost;
      H->color = (COLOR *) malloc (sizeof (COLOR) * n);
      H->pixel = (int *) malloc (sizeof (int) * n);
      H->pos   = (int *) malloc (sizeof (int) * n);
      H->last = NIL;
      H->removal_policy = REMOVAL_POLICY_MIN;
      if (H->color == NULL || H->pos == NULL || H->pixel == NULL)
        error (LOG_OUT_OF_MEMORY);
      for (i = 0; i < H->n; i++)
        {
          H->color[i] = COLOR_WHITE;
          H->pos[i]   = NIL;
          H->pixel[i] = NIL;
        }
    }
  else
    error (LOG_OUT_OF_MEMORY);

  return H;
}

void
real_heap_destroy (real_heap ** H)
{
  real_heap *aux = *H;
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

int
real_heap_insert (real_heap * H, int pixel)
{
  if (!real_heap_is_full (H))
    {
      H->last++;
      H->pixel[H->last] = pixel;
      H->color[pixel] = COLOR_GRAY;
      H->pos[pixel] = H->last;
      real_heap_go_up (H, H->last);
      return 1;
    }
  else
    return 0;
}

int
real_heap_remove (real_heap * H, int *pixel)
{
  if (!real_heap_is_empty (H))
    {
      *pixel = H->pixel[0];
      H->pos[*pixel] = NIL;
      H->color[*pixel] = COLOR_BLACK;
      H->pixel[0] = H->pixel[H->last];
      H->pos[H->pixel[0]] = 0;
      H->pixel[H->last] = NIL;
      H->last--;
      real_heap_go_down (H, 0);
      return 1;
    }
  else
    return 0;
}


void
real_heap_update (real_heap * H, int p, float value)
{
  H->cost[p] = value;

  if (H->color[p] == COLOR_BLACK)
    {
      printf ("error: p has been removed\n");
    }

  if (H->color[p] == COLOR_WHITE)
    real_heap_insert (H, p);
  else
    real_heap_go_up (H, H->pos[p]);
}

void
real_heap_reset (real_heap * H)
{
  int i;

  for (i = 0; i < H->n; i++)
    {
      H->color[i] = COLOR_WHITE;
      H->pos[i] = NIL;
      H->pixel[i] = NIL;
    }
  H->last = NIL;
}
