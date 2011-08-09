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

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

#include "common.h"
#include "set.h"
#include "subgraph.h"
#include "realheap.h"
#include "metrics.h"
#include "measures.h"
#include "knn.h"
#include "supervised.h"

// Find prototypes by the MST approach
void
mst_prototypes (subgraph * sg)
{
  int p, q;
  float weight;
  real_heap *Q = NULL;
  float *path_val = NULL;
  int pred;
  float nproto;

  // initialization
  path_val = alloc_float (sg->node_n);
  Q = real_heap_create (sg->node_n, path_val);

  for (p = 0; p < sg->node_n; p++)
    {
      path_val[p] = FLT_MAX;
      sg->node[p].status = 0;
    }

  path_val[0] = 0;
  sg->node[0].pred = NIL;
  real_heap_insert (Q, 0);

  nproto = 0.0;

  // Prim's algorithm for Minimum Spanning Tree
  while (!real_heap_is_empty (Q))
    {
      real_heap_remove (Q, &p);
      sg->node[p].path_val = path_val[p];

      pred = sg->node[p].pred;
      if (pred != NIL)
        if (sg->node[p].label_true != sg->node[pred].label_true)
          {
            if (sg->node[p].status != STATUS_PROTOTYPE)
              {
                sg->node[p].status = STATUS_PROTOTYPE;
                nproto++;
              }
            if (sg->node[pred].status != STATUS_PROTOTYPE)
              {
                sg->node[pred].status = STATUS_PROTOTYPE;
                nproto++;
              }
          }

      for (q = 0; q < sg->node_n; q++)
        {
          if (Q->color[q] != COLOR_BLACK)
            {
              if (p != q)
                {
                  if (!sg->use_precomputed_distance)
                    weight = sg->arc_weight (sg->node[p].feat, sg->node[q].feat, sg->feat_n);
                  else
                    weight = sg->distance_value[sg->node[p].position][sg-> node[q].position];

                  if (weight < path_val[q])
                    {
                      sg->node[q].pred = p;
                      real_heap_update (Q, q, weight);
                    }
                }
            }
        }
    }
  real_heap_destroy (&Q);
  free (path_val);

}

void
supervised_train (subgraph * sg)
{
  int p, q, i;
  float tmp, weight;
  real_heap *Q = NULL;
  float *path_val = NULL;

  // compute optimum prototypes
  mst_prototypes (sg);

  // initialization
  path_val = alloc_float (sg->node_n);

  Q = real_heap_create (sg->node_n, path_val);

  for (p = 0; p < sg->node_n; p++)
    {
      if (sg->node[p].status == STATUS_PROTOTYPE)
        {
          sg->node[p].pred = NIL;
          path_val[p] = 0;
          sg->node[p].label = sg->node[p].label_true;
          real_heap_insert (Q, p);
        }
      else                      // non-prototypes
        {
          path_val[p] = FLT_MAX;
        }
    }

  // IFT with fmax
  i = 0;
  while (!real_heap_is_empty (Q))
    {
      real_heap_remove (Q, &p);

      sg->ordered_list_of_nodes[i] = p;
      i++;
      sg->node[p].path_val = path_val[p];

      for (q = 0; q < sg->node_n; q++)
        {
          if (p != q)
            {
              if (path_val[p] < path_val[q])
                {
                  if (!sg->use_precomputed_distance)
                    weight = sg->arc_weight (sg->node[p].feat, sg->node[q].feat, sg->feat_n);
                  else
                    weight = sg->distance_value[sg->node[p].position][sg->node[q].position];

                  tmp = MAX (path_val[p], weight);
                  if (tmp < path_val[q])
                    {
                      sg->node[q].pred = p;
                      sg->node[q].label = sg->node[p].label;
                      real_heap_update (Q, q, tmp);
                    }
                }
            }
        }
    }

  real_heap_destroy (&Q);
  free (path_val);
}



//Classification function: it simply classifies samples from sg -----
void
supervised_classify (subgraph * sg_train, float *feat, int sample_n, int *label)
{
  int i, j, k, l, c_label = -1;
  float tmp, weight, minCost;

  for (i = 0; i < sample_n; i++)
    {
      j = 0;
      k = sg_train->ordered_list_of_nodes[j];

      if (!sg_train->use_precomputed_distance)
        weight = sg_train->arc_weight (sg_train->node[k].feat, &feat[i*sg_train->feat_n], sg_train->feat_n);
      else
        weight = sg_train->distance_value[sg_train->node[k].position][i];

      minCost = MAX (sg_train->node[k].path_val, weight);
      c_label = sg_train->node[k].label;

      while ((j < sg_train->node_n - 1) &&
             (minCost > sg_train->node[sg_train->ordered_list_of_nodes[j + 1]].path_val))
        {
          l = sg_train->ordered_list_of_nodes[j + 1];

          if (!sg_train->use_precomputed_distance)
            weight = sg_train->arc_weight (sg_train->node[k].feat, &feat[i*sg_train->feat_n], sg_train->feat_n);
          else
            weight = sg_train->distance_value[sg_train->node[k].position][i];

          tmp = MAX (sg_train->node[l].path_val, weight);
          if (tmp < minCost)
            {
              minCost = tmp;
              c_label = sg_train->node[l].label;
            }
          j++;
          k = l;
        }
      label[i] = c_label;
    }
}
