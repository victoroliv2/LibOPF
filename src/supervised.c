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

#include <omp.h>

// Find prototypes by the MST approach
static void
mst_prototypes (struct subgraph * sg)
{
  int p, q;
  float weight;
  struct real_heap *Q = NULL;
  float *path_val = NULL;
  int pred;
  float nproto;

  // initialization
  path_val = alloc_float (sg->node_n);
  Q = real_heap_create (sg->node_n, path_val);

  for (p = 0; p < sg->node_n; p++)
    {
      path_val[p] = FLT_MAX;
      sg->node[p].status = STATUS_NOTHING;
    }

  path_val[0] = 0;
  sg->node[0].pred = NIL;
  real_heap_insert (Q, 0);

  nproto = 0.0f;

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
                  weight = subgraph_get_distance (sg, &sg->node[p], &sg->node[q]);

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
opf_supervised_train (struct subgraph * sg)
{
  int p, q, i;
  float tmp, weight;
  struct real_heap *Q = NULL;
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
                  weight = subgraph_get_distance (sg, &sg->node[p], &sg->node[q]);

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
opf_supervised_classify (struct subgraph * sg_train, float *feat, int sample_n, int *label)
{
  int i;

  omp_set_num_threads(NTHREADS);
  #pragma omp parallel for
  for (i = 0; i < sample_n; i++)
    {
      int c_label = -1;
      int j = 0;
      float minCost = FLT_MAX;

      for (j=0;

           (j < sg_train->node_n)
           && (minCost > sg_train->node[sg_train->ordered_list_of_nodes[j]].path_val);

          j++)
        {
          int l;
          float tmp, weight;

          l = sg_train->ordered_list_of_nodes[j];

          weight = sg_train->arc_weight
                     (sg_train->node[l].feat, &feat[i*sg_train->feat_n], sg_train->feat_n);

          tmp = MAX (sg_train->node[l].path_val, weight);
          if (tmp < minCost)
            {
              minCost = tmp;
              c_label = sg_train->node[l].label;
            }
        }

      label[i] = c_label;
    }
}


static void
supervised_classify_subgraph (struct subgraph * sg_train, struct subgraph * sg_eval)
{
  int i;

  omp_set_num_threads(NTHREADS);
  #pragma omp parallel for
  for (i = 0; i < sg_eval->node_n; i++)
    {
      int c_label = -1;
      int j = 0;
      float minCost = FLT_MAX;

      for (j=0;

           (j < sg_train->node_n)
           && (minCost > sg_train->node[sg_train->ordered_list_of_nodes[j]].path_val);

          j++)
        {
          int l;
          float tmp, weight;

          l = sg_train->ordered_list_of_nodes[j];

          weight = sg_train->arc_weight
                     (sg_train->node[l].feat, sg_eval->node[i].feat, sg_train->feat_n);

          tmp = MAX (sg_train->node[l].path_val, weight);
          if (tmp < minCost)
            {
              minCost = tmp;
              c_label = sg_train->node[l].label;
            }
        }

      sg_eval->node[i].label = c_label;
    }
}


static float
accuracy (struct subgraph *sg)
{
  int ok = 0;
  int i;

  for (i=0; i < sg->node_n; i++)
    (sg->node[i].label == sg->node[i].label_true)? ok++ : 0;

  return (float)(ok)/(float)(sg->node_n);
}

/* Replace errors from evaluating set by non prototypes from training set */
static void
swap_wrong_prototypes (struct subgraph *sg_train, struct subgraph *sg_eval)
{
  int i;
  int nonprototypes = 0;

  for (i = 0; i < sg_train->node_n; i++)
    if (sg_train->node[i].status == STATUS_NOTHING) nonprototypes++;

  while (i < sg_eval->node_n && nonprototypes > 0)
    {
      if (sg_eval->node[i].label != sg_eval->node[i].label_true)
        {
          /* XXX: this can take a lot of time */
          int j;
          do
            {
              j = random_int (0, sg_train->node_n);
            }
          while (sg_train->node[j].status == STATUS_PROTOTYPE);

          {
            struct snode tmp  = sg_train->node[j];
            sg_train->node[j] = sg_eval->node[i];
            sg_eval->node[i]  = tmp;
          }

          sg_train->node[j].pred = NIL;
          sg_train->node[j].status = STATUS_PROTOTYPE;

          nonprototypes--;
        }
      i++;
    }
}

/* create two pointers to a subgraph data,
   this function is used in some training modes to reorganize nodes */
static void
subgraph_split_mirrored (struct subgraph * sg, float split,
                         struct subgraph * sg1, struct subgraph * sg2)
{
  memset (sg1, 0xFF, sizeof(struct subgraph));
  memset (sg2, 0xFF, sizeof(struct subgraph));

  sg1->node_n = sg->node_n * split;
  sg2->node_n = sg->node_n - sg1->node_n;

  sg1->feat_n = sg2->feat_n = sg->feat_n;

  sg1->node = &sg->node[0];
  sg2->node = &sg->node[sg1->node_n];

  sg1->ordered_list_of_nodes = &sg->ordered_list_of_nodes[0];
  sg2->ordered_list_of_nodes = &sg->ordered_list_of_nodes[sg1->node_n];

  sg1->arc_weight = sg2->arc_weight = sg->arc_weight;
  sg1->feat_data = sg2->feat_data = sg->feat_data;
}

#define ITER_MAX 10

void
opf_supervised_train_iterative (struct subgraph *sg, float split)
{
  int i = 0;
  float acc = FLT_MIN, acc_prev = FLT_MIN, delta;

  struct subgraph sg_train, sg_eval;
  subgraph_split_mirrored (sg, split, &sg_train, &sg_eval);

  do
    {
      acc_prev = acc;

      opf_supervised_train (&sg_train);
      supervised_classify_subgraph (&sg_train, &sg_eval);
      acc = accuracy (&sg_eval);

      swap_wrong_prototypes (&sg_train, &sg_eval);

      delta = fabs(acc-acc_prev);
      i++;
    }
  while ((delta > FLT_EPSILON) && (i < ITER_MAX));
}

/* Move misclassified data from eval to sg */
static void
move_misclassified_nodes (struct subgraph *sg_train, struct subgraph *sg_eval, int *n)
{
  int i;
  int misclassified_n = 0;

  /* count number of misclassied samples in eval */
  for (i=0; i < sg_eval->node_n; i++)
    (sg_eval->node[i].label != sg_eval->node[i].label_true)?
      misclassified_n++ : 0;

  *n = misclassified_n;

  /* nothing to do */
  if (misclassified_n == 0)
    return;

  /* move wrong labelled samples from sg_eval to sg_train */
  for (i=0; i < sg_eval->node_n; i++)
    {
      if (sg_eval->node[i].label != sg_eval->node[i].label_true)
        {
          /* have in mind that sg_train and sg_eval
             are mirrorred to the same subgraph, so
             sg_train->node[sg_train->node_n] == sg_eval->node[0] */
          {
            struct snode tmp = sg_train->node[sg_train->node_n];
            sg_train->node[sg_train->node_n] = sg_eval->node[i];
            sg_eval->node[i] = tmp;
          }

          sg_train->node_n++;
          sg_eval->node_n--;

          /* sg_train took the first element of sg_eval */
          sg_eval->node++;
          sg_eval->ordered_list_of_nodes++;
        }
    }
}

void
opf_supervised_train_agglomerative (struct subgraph *sg, float split)
{
    int n;
    struct subgraph sg_train, sg_eval;
    subgraph_split_mirrored (sg, split, &sg_train, &sg_eval);

    /* while there exists misclassified samples in eval */
    do
      {
        n = 0;
        opf_supervised_train (sg);
        supervised_classify_subgraph (&sg_train, &sg_eval);
        move_misclassified_nodes (&sg_train, &sg_eval, &n);
      }
    while(n > 0);
}
