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

#include "supervised.h"

//Replace errors from evaluating set by non prototypes from training set
void
swap_errors_by_non_prototypes (subgraph ** sg_train, subgraph ** sg_eval)
{
  int i, j, counter, non_prototypes = 0, error_n = 0;

  for (i = 0; i < (*sg_train)->node_n; i++)
    {
      if ((*sg_train)->node[i].pred != NIL)      // non prototype
        {
          non_prototypes++;
        }
    }

  for (i = 0; i < (*sg_eval)->node_n; i++)
    if ((*sg_eval)->node[i].label != (*sg_eval)->node[i].label_true)
      error_n++;

  for (i = 0; i < (*sg_eval)->node_n && non_prototypes > 0 && error_n > 0; i++)
    {
      if ((*sg_eval)->node[i].label != (*sg_eval)->node[i].label_true)
        {
          counter = non_prototypes;
          while (counter > 0)
            {
              j = RandomInteger (0, (*sg_train)->node_n - 1);
              if ((*sg_train)->node[j].pred != NIL)
                {
                  snode_swap (&((*sg_train)->node[j]), &((*sg_eval)->node[i]));
                  (*sg_train)->node[j].pred = NIL;
                  non_prototypes--;
                  error_n--;
                  counter = 0;
                }
              else
                counter--;
            }
        }
    }
}

//mark nodes and the whole path as relevants
void
mark_nodes (subgraph * g, int i)
{
  while (g->node[i].pred != NIL)
    {
      g->node[i].relevant = 1;
      i = g->node[i].pred;
    }
  g->node[i].relevant = 1;
}

// Remove irrelevant nodes
void
remove_irrelevant_nodes (subgraph ** sg)
{
  subgraph *newsg = NULL;
  int i, k, num_of_irrelevants = 0;

  for (i = 0; i < (*sg)->node_n; i++)
    {
      if (!(*sg)->node[i].relevant)
        num_of_irrelevants++;
    }

  if (num_of_irrelevants > 0)
    {
      newsg = subgraph_create ((*sg)->node_n - num_of_irrelevants);
      newsg->feat_n = (*sg)->feat_n;
//    for (i=0; i < newsg->node_n; i++)
//      newsg->node[i].feat = AllocFloatArray(newsg->feat_n);

      k = 0;
      newsg->label_n = (*sg)->label_n;
      for (i = 0; i < (*sg)->node_n; i++)
        {
          if ((*sg)->node[i].relevant)  // relevant node
            {
              snode_copy (&(newsg->node[k]), &((*sg)->node[i]), newsg->feat_n);
              k++;
            }
        }
      newsg->label_n = (*sg)->label_n;
      subgraph_destroy (sg);
      *sg = newsg;
    }
}

//Move irrelevant nodes from source graph (src) to destiny graph (dst)
void
move_irrelevant_nodes (subgraph ** src, subgraph ** dst)
{
  int i, j, k, num_of_irrelevants = 0;
  subgraph *newsrc = NULL, *newdst = NULL;

  for (i = 0; i < (*src)->node_n; i++)
    {
      if (!(*src)->node[i].relevant)
        num_of_irrelevants++;
    }

  if (num_of_irrelevants > 0)
    {
      newsrc = subgraph_create ((*src)->node_n - num_of_irrelevants);
      newdst = subgraph_create ((*dst)->node_n + num_of_irrelevants);

      newsrc->feat_n = (*src)->feat_n;
      newdst->feat_n = (*dst)->feat_n;
      newsrc->label_n = (*src)->label_n;
      newdst->label_n = (*dst)->label_n;

//    for (i=0; i < newsrc->node_n; i++)
//      newsrc->node[i].feat = AllocFloatArray(newsrc->feat_n);

//    for (i=0; i < newdst->node_n; i++)
//      newdst->node[i].feat = AllocFloatArray(newdst->feat_n);

      for (i = 0; i < (*dst)->node_n; i++)
        snode_copy (&(newdst->node[i]), &((*dst)->node[i]), newdst->feat_n);
      j = i;

      k = 0;
      for (i = 0; i < (*src)->node_n; i++)
        {
          if ((*src)->node[i].relevant) // relevant node
            snode_copy (&(newsrc->node[k++]), &((*src)->node[i]),
                       newsrc->feat_n);
          else
            snode_copy (&(newdst->node[j++]), &((*src)->node[i]),
                       newdst->feat_n);
        }
      subgraph_destroy (&(*src));
      subgraph_destroy (&(*dst));
      *src = newsrc;
      *dst = newdst;
    }
}

//Move misclassified nodes from source graph (src) to destiny graph (dst)
void
move_misclassified_nodes (subgraph ** src, subgraph ** dst, int *p)
{
  int i, j, k, num_of_misclassified = 0;
  subgraph *newsrc = NULL, *newdst = NULL;

  for (i = 0; i < (*src)->node_n; i++)
    {
      if ((*src)->node[i].label_true != (*src)->node[i].label)
        num_of_misclassified++;
    }
  *p = num_of_misclassified;

  if (num_of_misclassified > 0)
    {
      newsrc = subgraph_create ((*src)->node_n - num_of_misclassified);
      newdst = subgraph_create ((*dst)->node_n + num_of_misclassified);

      newsrc->feat_n = (*src)->feat_n;
      newdst->feat_n = (*dst)->feat_n;
      newsrc->label_n = (*src)->label_n;
      newdst->label_n = (*dst)->label_n;

      for (i = 0; i < (*dst)->node_n; i++)
        snode_copy (&(newdst->node[i]), &((*dst)->node[i]), newdst->feat_n);
      j = i;

      k = 0;
      for (i = 0; i < (*src)->node_n; i++)
        {
          if ((*src)->node[i].label_true == (*src)->node[i].label)       // misclassified node
            snode_copy (&(newsrc->node[k++]), &((*src)->node[i]),
                       newsrc->feat_n);
          else
            snode_copy (&(newdst->node[j++]), &((*src)->node[i]),
                       newdst->feat_n);
        }
      subgraph_destroy (&(*src));
      subgraph_destroy (&(*dst));
      *src = newsrc;
      *dst = newdst;
    }
}

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
  path_val = AllocFloatArray (sg->node_n);
  Q = create_real_heap (sg->node_n, path_val);

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
                  if (!use_precomputed_distance)
                    weight =
                      arc_weight (sg->node[p].feat, sg->node[q].feat,
                                     sg->feat_n);
                  else
                    weight =
                      distance_value[sg->node[p].position][sg->
                                                              node
                                                              [q].position];
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

//Training function -----
void
supervised_training (subgraph * sg)
{
  int p, q, i;
  float tmp, weight;
  real_heap *Q = NULL;
  float *path_val = NULL;

  // compute optimum prototypes
  mst_prototypes (sg);

  // initialization
  path_val = AllocFloatArray (sg->node_n);

  Q = create_real_heap (sg->node_n, path_val);

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
                  if (!use_precomputed_distance)
                    weight =
                      arc_weight (sg->node[p].feat, sg->node[q].feat,
                                     sg->feat_n);
                  else
                    weight =
                      distance_value[sg->node[p].position][sg->
                                                              node
                                                              [q].position];
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
supervised_classifying (subgraph * sg_train, subgraph * sg)
{
  int i, j, k, l, label = -1;
  float tmp, weight, minCost;

  for (i = 0; i < sg->node_n; i++)
    {
      j = 0;
      k = sg_train->ordered_list_of_nodes[j];
      if (!use_precomputed_distance)
        weight =
          arc_weight (sg_train->node[k].feat, sg->node[i].feat, sg->feat_n);
      else
        weight =
          distance_value[sg_train->node[k].position][sg->node[i].position];

      minCost = MAX (sg_train->node[k].path_val, weight);
      label = sg_train->node[k].label;

      while ((j < sg_train->node_n - 1) &&
             (minCost >
              sg_train->node[sg_train->ordered_list_of_nodes[j + 1]].path_val))
        {

          l = sg_train->ordered_list_of_nodes[j + 1];

          if (!use_precomputed_distance)
            weight =
              arc_weight (sg_train->node[l].feat, sg->node[i].feat,
                             sg->nfeats);
          else
            weight =
              distance_value[sg_train->node[l].position][sg->
                                                           node[i].position];
          tmp = MAX (sg_train->node[l].path_val, weight);
          if (tmp < minCost)
            {
              minCost = tmp;
              label = sg_train->node[l].label;
            }
          j++;
          k = l;
        }
      sg->node[i].label = label;
    }
}


//Learning function: it executes the learning procedure for CompGraph replacing the
//missclassified samples in the evaluation set by non prototypes from
//training set -----
void
supervised_learning (subgraph ** sg_train, subgraph ** sg_eval)
{
  int i = 0, iterations = 10;
  float Acc = FLT_MIN, AccAnt = FLT_MIN, MaxAcc = FLT_MIN, delta;
  subgraph *sg = NULL;

  do
    {
      AccAnt = Acc;
      fflush (stdout);
      fprintf (stdout, "\nrunning iteration ... %d ", i);
      supervised_training (*sg_train);
      supervised_training (*sg_train, *sg_eval);
      Acc = subgraph_accuracy (*sg_eval);
      if (Acc > MaxAcc)
        {
          MaxAcc = Acc;
          if (sg != NULL)
            subgraph_destroy (&sg);
          sg = subgraph_copy (*sg_train);
        }
      swap_errors_by_non_prototypes (&(*sg_train), &(*sg_eval));
      fflush (stdout);
      fprintf (stdout, "accuracy in the evaluation set: %.2f %%\n",
               Acc * 100);
      i++;
      delta = fabs (Acc - AccAnt);
    }
  while ((delta > 0.0001) && (i <= iterations));
  subgraph_destroy (&(*sg_train));
  *sg_train = sg;
}


void
supervised_agglomerative_learning (subgraph ** sg_train, subgraph ** sg_eval)
{
  int n, i = 1;
  float Acc;

  /*while  there exists misclassified samples in sg_eval */
  do
    {
      fflush (stdout);
      fprintf (stdout, "\nrunning iteration ... %d ", i++);
      n = 0;
      supervised_training (*sg_train);
      supervised_classifying (*sg_train, *sg_eval);
      Acc = subgraph_accuracy (*sg_eval);
      fprintf (stdout, " %f", Acc * 100);
      move_misclassified_nodes (&(*sg_eval), &(*sg_train), &n);
      fprintf (stdout, "\nMisclassified nodes: %d", n);
    }
  while (n);
}
