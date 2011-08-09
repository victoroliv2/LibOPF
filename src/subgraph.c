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
#include "subgraph.h"

/*----------- Constructor and destructor ------------------------*/
//Copy nodes
void
snode_copy (struct snode * dest, struct snode * src, int feat_n)
{
  dest->feat = alloc_float (feat_n);
  memcpy (dest->feat, src->feat, feat_n * sizeof (float));
  dest->path_val = src->path_val;
  dest->dens = src->dens;
  dest->label = src->label;
  dest->root = src->root;
  dest->pred = src->pred;
  dest->label_true = src->label_true;
  dest->position = src->position;
  dest->status = src->status;
  dest->relevant = src->relevant;
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
struct subgraph *
subgraph_create (int node_n)
{
  struct subgraph *sg = (struct subgraph *) calloc (1, sizeof (struct subgraph));
  int i;

  sg->node_n = node_n;
  sg->node = (struct snode *) calloc (node_n, sizeof (struct snode));
  sg->ordered_list_of_nodes = (int *) calloc (node_n, sizeof (int));

  if (sg->node == NULL)
    error (LOG_OUT_OF_MEMORY);

  for (i = 0; i < sg->node_n; i++)
    {
      sg->node[i].feat = NULL;
      sg->node[i].relevant = 0;
    }

  return (sg);
}

// Deallocate memory for subgraph
void
subgraph_destroy (struct subgraph ** sg)
{
  int i;

  if ((*sg) != NULL)
    {
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

void
subgraph_set_metric (struct subgraph *sg, enum METRIC m)
{
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
    default:
      error ("Undefinied metric");
  }
}

// subgraph_pdf_evaluate computation
void
subgraph_pdf_evaluate (struct subgraph * sg)
{
  int i, nelems;
  double dist;
  float *value = alloc_float (sg->node_n);
  struct set *adj = NULL;

  sg->k = (2.0 * (float) sg->df / 9.0);
  sg->dens_min = FLT_MAX;
  sg->dens_max = FLT_MIN;
  for (i = 0; i < sg->node_n; i++)
    {
      adj = sg->node[i].adj;
      value[i] = 0.0;
      nelems = 1;
      while (adj != NULL)
        {
          if (!sg->use_precomputed_distance)
            dist = sg->arc_weight (sg->node[i].feat, sg->node[adj->elem].feat, sg->feat_n);
          else
            dist = sg->distance_value[sg->node[i].position][sg->node[adj->elem].position];

          value[i] += exp (-dist / sg->k);
          adj = adj->next;
          nelems++;
        }

      value[i] = (value[i] / (float) nelems);

      if (value[i] < sg->dens_min)
        sg->dens_min = value[i];
      if (value[i] > sg->dens_max)
        sg->dens_max = value[i];
    }

  //  printf("df=%f,K1=%f,K2=%f,dens_min=%f, dens_max=%f\n",sg->df,sg->K1,sg->K2,sg->dens_min,sg->dens_max);

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
            ((float) (DENS_MAX - 1) * (value[i] - sg->dens_min) /
             (float) (sg->dens_max - sg->dens_min)) + 1.0;
          sg->node[i].path_val = sg->node[i].dens - 1;
        }
    }
  free (value);
}
