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
void
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
supervised_train (struct subgraph * sg)
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
supervised_classify (struct subgraph * sg_train, float *feat, int sample_n, int *label)
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

static float
accuracy (int *label, int *label_truth, int n)
{
  int ok = 0;
  int i;

  for (i=0; i < n; i++)
    (label[i] != label_truth[i])? ok++ : 0;

  return (float)(ok)/(float)(n);
}

//Replace errors from evaluating set by non prototypes from training set
static void
swap_wrong_prototypes (struct subgraph *sg, float *eval_feat, int *eval_label,
                       int eval_n, int *label)
{
  int i;
  int nonprototypes = 0;

  float *feat_buf = (float *) calloc (sg->feat_n, sizeof (float));

  for (i = 0; i < sg->node_n; i++)
    if (sg->node[i].status == STATUS_NOTHING) nonprototypes++;

  while (i < eval_n && nonprototypes > 0)
    {
      if (eval_label[i] != label[i])
        {
          /* XXX: this can take a lot of time */
          int j;
          do
            {
              j = random_int (0, sg->node_n);
            }
          while (sg->node[j].status == STATUS_PROTOTYPE);

          sg->node[j].pred = NIL;
          sg->node[j].status = STATUS_PROTOTYPE;

          /* exchange feature vectors */
          memcpy (feat_buf, sg->node[j].feat, sg->feat_n*sizeof (float));
          memcpy (sg->node[j].feat, &eval_feat[sg->feat_n*i], sg->feat_n*sizeof (float));
          memcpy (&eval_feat[sg->feat_n*i], feat_buf, sg->feat_n*sizeof (float));

          /* update all distances */
          if (sg->use_precomputed_distance)
            {
              for (i = 0; i < sg->node_n; i++)
                {
                  sg->distance_value[i*sg->node_n+j] =
                    sg->arc_weight (sg->node[i].feat, sg->node[j].feat, sg->feat_n);

                  sg->distance_value[j*sg->node_n+i] =
                    sg->arc_weight (sg->node[j].feat, sg->node[i].feat, sg->feat_n);
                }
            }

          nonprototypes--;
        }
      i++;
    }

  free (feat_buf);
}

#define ITER_MAX 10

void
supervised_training_iterative (struct subgraph *sg, float *eval_feat, int *eval_label,
                               int eval_n)
{
  int i = 0;
  float acc = FLT_MIN, acc_prev = FLT_MIN, delta;
  int *label = (int *) calloc (eval_n, sizeof (int));

  do
    {
      acc_prev = acc;

      supervised_train (sg);
      supervised_classify (sg, eval_feat, eval_n, label);
      acc = accuracy (eval_label, label, eval_n);

      swap_wrong_prototypes (sg, eval_feat, eval_label, eval_n, label);

      delta = fabs(acc-acc_prev);
      i++;
    }
  while ((delta > FLT_EPSILON) && (i < ITER_MAX));

  free(label);
}

/* Move misclassified data from eval to sg */
static void
move_misclassified_nodes (struct subgraph *sg, float *eval_feat, int *eval_label,
                          int eval_n, int *label, int *n)
{
  int i,j;
  int misclassified_n = 0;
  int old_n = sg->node_n;

  /* count number of misclassied samples in eval */
  for (i=0; i < sg->node_n; i++)
    (eval_label[i] != NIL && eval_label[i] != label[i])? misclassified_n++ : 0;

  *n = misclassified_n;

  /* nothing to do */
  if (misclassified_n == 0)
    return;

  /* move wrong labelled samples to subgraph */
  for (i=0; i < eval_n; i++)
    {
      if (eval_label[i] != NIL && eval_label[i] != label[i])
        {
          sg->node[old_n+i].label_true = eval_label[i];

          /* bring feature vector to subgraph */
          memcpy (sg->node[old_n+i].feat, &eval_feat[sg->feat_n*i],
                  sg->feat_n*sizeof (float));

          eval_label[i] = NIL; /* don't use this sample in next iterations */

          /* 0xFF (nan) for sanity! */
          memset (&eval_feat[sg->feat_n*i], 0xFF, sg->feat_n*sizeof(float));
        }
    }

  /* distance table new values */
  if (sg->use_precomputed_distance)
    {
      for (i=0; i < old_n; i++)
        for (j=old_n; j < sg->node_n; j++)
          sg->distance_value[sg->node_n*i+j] =
             sg->arc_weight (sg->node[i].feat, sg->node[j].feat, sg->feat_n);

      for (i=old_n; i < sg->node_n; i++)
        for (j=0; j < sg->node_n; j++)
          sg->distance_value[sg->node_n*i+j] =
            sg->arc_weight (sg->node[i].feat, sg->node[j].feat, sg->feat_n);
    }
}

void
supervised_training_agglomerative (struct subgraph *sg,
                                   float *eval_feat, int *eval_label, int eval_n)
{
    int n;
    int *label = (int *) calloc (eval_n, sizeof (int));

    /* while there exists misclassified samples in eval */
    do
      {
        n = 0;
        supervised_train (sg);
        supervised_classify (sg, eval_feat, eval_n, label);

        move_misclassified_nodes (sg, eval_feat, eval_label, eval_n, label, &n);
      }
    while(n > 0);

  free(label);
}
