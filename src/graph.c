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

#include <assert.h>
#include <time.h>

#include "common.h"
#include "knn.h"
#include "metrics.h"
#include "graph.h"

/*----------- Constructor and destructor ------------------------*/
//Copy nodes

void
snode_clear (struct snode *n)
{
  n->path_val   = NAN;
  n->dens       = NAN;
  n->radius     = NAN;
  n->label      = NIL;
  n->root       = NIL;
  n->pred       = NIL;
  n->label_true = NIL;
  n->position   = NIL;
  n->feat       = NULL;
  n->status     = 0;
  n->nplatadj   = 0;
  n->adj        = NULL;
}

void
snode_copy (struct snode * dest, struct snode * src, int feat_n)
{
  dest->feat = alloc_double (feat_n);
  memcpy (dest->feat, src->feat, feat_n * sizeof (double));
  dest->path_val = src->path_val;
  dest->dens = src->dens;
  dest->label = src->label;
  dest->root = src->root;
  dest->pred = src->pred;
  dest->label_true = src->label_true;
  dest->position = src->position;
  dest->status = src->status;
  dest->radius = src->radius;
  dest->nplatadj = src->nplatadj;

  dest->adj = set_clone (src->adj);
}

//Swap nodes
void
snode_swap (struct snode * a, struct snode * b)
{
  struct snode tmp;

  tmp = *a;
  *a = *b;
  *b = tmp;
}

// Allocate nodes without features
struct opf_graph *
opf_graph_create (int node_n)
{
  struct opf_graph *sg = (struct opf_graph *) calloc (1, sizeof (struct opf_graph));

  int i;

  if (!sg) return NULL;

  memset (sg, 0xFF, sizeof (struct opf_graph));

  sg->node_n = node_n;

  sg->node = (struct snode *) calloc (node_n, sizeof (struct snode));
  sg->ordered_list_of_nodes = (int *) calloc (node_n, sizeof (int));

  if (!(sg->node && sg->ordered_list_of_nodes)) return NULL;

  /* sanity */
  memset (sg->node, 0xFF, node_n*sizeof (struct snode));
  memset (sg->ordered_list_of_nodes, 0xFF, node_n*sizeof (int));

  for (i = 0; i < sg->node_n; i++)
    {
      snode_clear(&sg->node[i]);
      sg->node[i].position = i;
    }

  sg->feat_data = NULL;

  sg->pdist = NULL;
  sg->pdist_train_stride    = INT_MAX; /* i want to segfault if it isn't used properly */

  return (sg);
}

// Deallocate memory for opf_graph
void
opf_graph_destroy (struct opf_graph ** sg)
{
  int i;

  if ((*sg) != NULL)
    {
      if ((*sg)->feat_data)
        free ((*sg)->feat_data);

      if ((*sg)->pdist)
        free ((*sg)->pdist);

      for (i = 0; i < (*sg)->node_n; i++)
        {
          if ((*sg)->node[i].adj != NULL)
            set_destroy (&(*sg)->node[i].adj);
        }
      free ((*sg)->node);
      free ((*sg)->ordered_list_of_nodes);
      free ((*sg));
      *sg = NULL;
    }
}

int
opf_graph_set_feature (struct opf_graph *sg, double *feat, int *label, int feat_n)
{
  int i;
  sg->feat_n = feat_n;

  sg->feat_data = (double *) calloc (sg->node_n*sg->feat_n, sizeof(double));

  if (!sg->feat_data) return FALSE;

  // 0xFF (nan) for sanity!
  memset (sg->feat_data, 0xFF, sg->node_n*sg->feat_n*sizeof(double));

  for (i = 0; i < sg->node_n; i++)
    {
      double *chunk = &sg->feat_data[i*sg->feat_n];
      memcpy (chunk, &feat[sg->feat_n*i], sg->feat_n*sizeof(double));
      sg->node[i].feat  = chunk;
      if (label) sg->node[i].label_true = label[i];
    }

  return TRUE;
}

void
opf_graph_set_metric (struct opf_graph *sg,
                     double (*arc_weight) (double *f1, double *f2, int n),
                     enum METRIC m)
{
  if (arc_weight)
    {
      sg->arc_weight = arc_weight;
      return;
    }

  switch (m)
  {
    case EUCLIDIAN:
      sg->arc_weight = d_eucl;
      break;
    case LOG_EUCLIDIAN:
      sg->arc_weight = d_eucl_log;
      break;
/*    case GAUSSIAN:
      sg->arc_weight = d_gauss;
      break;*/
    case CHI_SQUARE:
      sg->arc_weight = d_chi_square;
      break;
    case MANHATTAN:
      sg->arc_weight = d_manhattan;
      break;
    case CANBERRA:
      sg->arc_weight = d_canberra;
      break;
    case SQUARED_CHORD:
      sg->arc_weight = d_squared_chord;
      break;
    case SQUARED_CHI_SQUARE:
      sg->arc_weight = d_squared_chi_square;
      break;
    case BRAY_CURTIS:
      sg->arc_weight = d_bray_curtis;
      break;
    case NO_METRIC:
      sg->arc_weight = NULL;
      break;
    default:
      error ("Undefinied metric");
  }
}


int
opf_graph_set_precomputed_distance (struct opf_graph *sg,
                                   double *dist,
                                   int *label)
{
  int i;

  sg->pdist = (double *) calloc (sg->node_n*sg->node_n, sizeof(double));
  sg->pdist_train_stride = sg->node_n;

  if (!sg->pdist) return FALSE;

  memcpy (sg->pdist, dist, sg->node_n*sg->node_n*sizeof(double));

  if (label)
    for (i = 0; i < sg->node_n; i++)
      sg->node[i].label_true = label[i];

  return TRUE;
}


// opf_graph_pdf_evaluate computation
void
opf_graph_pdf_evaluate (struct opf_graph * sg)
{
  int i, nelems;
  double dist;
  double *value = alloc_double (sg->node_n);
  struct set *adj = NULL;

  sg->k = (2.0 * (double) sg->df / 9.0);
  sg->dens_min = FLT_MAX;
  sg->dens_max = FLT_MIN;
  for (i = 0; i < sg->node_n; i++)
    {
      adj = sg->node[i].adj;
      value[i] = 0.0;
      nelems = 1;
      while (adj != NULL)
        {
          dist = opf_graph_get_distance (sg, &sg->node[i], &sg->node[adj->elem]);

          value[i] += exp (-dist / sg->k);
          adj = adj->next;
          nelems++;
        }

      value[i] = (value[i] / (double) nelems);

      if (value[i] < sg->dens_min)
        sg->dens_min = value[i];
      if (value[i] > sg->dens_max)
        sg->dens_max = value[i];
    }

  if (sg->dens_min == sg->dens_max)
    {
      for (i = 0; i < sg->node_n; i++)
        {
          sg->node[i].dens = DENS_MAX;
          sg->node[i].path_val = DENS_MAX - 1;
        }
    }
  else
    {
      for (i = 0; i < sg->node_n; i++)
        {
          sg->node[i].dens =
            ((double) (DENS_MAX - 1) * (value[i] - sg->dens_min) /
             (double) (sg->dens_max - sg->dens_min)) + 1.0;
          sg->node[i].path_val = sg->node[i].dens - 1;
        }
    }
  free (value);
}

/* this function doesn't update the new values in the distance table */
void
opf_graph_resize (struct opf_graph * sg, int node_n)
{
  int i;
  int old_n = sg->node_n;

  sg->node_n = node_n;
  sg->node                  = (struct snode *) realloc (sg->node,
                                                        sg->node_n*sizeof (struct snode));
  sg->ordered_list_of_nodes = (int *)          realloc (sg->ordered_list_of_nodes,
                                                        sg->node_n*sizeof (int));
  if (sg->feat_data)
    sg->feat_data           = (double *)        realloc (sg->feat_data,
                                                        sg->node_n*sg->feat_n*sizeof(double));

  /* sanity */
  if (node_n > old_n)
    {
      memset (&sg->node[old_n],                   0xFF, (sg->node_n-old_n)*sizeof (struct snode));
      memset (&sg->ordered_list_of_nodes[old_n],  0xFF, (sg->node_n-old_n)*sizeof (int));

      if (sg->feat_data)
        memset (&sg->feat_data[old_n*sg->feat_n], 0xFF, (sg->node_n-old_n)*sizeof (double));
    }

  /* clear new nodes */
  for (i=old_n; i < sg->node_n; i++)
    {
      snode_clear(&sg->node[i]);
      sg->node[i].position = i;
    }

  /* feat_data can change in realloc */
  if (sg->feat_data)
    for (i=0; i < sg->node_n; i++)
      sg->node[i].feat = &sg->feat_data[i*sg->feat_n];

  if (sg->pdist)
    {
      int i,j;

      double *dt = (double *) calloc (sg->node_n*sg->node_n, sizeof (double));

      for (i=0; i < sg->node_n; i++)
        for (j=0; j < sg->node_n; j++)
          dt[i*node_n+j] = PDISTANCE(sg, sg->node[i].position, sg->node[j].position);

      free(sg->pdist);

      sg->pdist = dt;
      sg->pdist_train_stride = sg->node_n;
    }
}
