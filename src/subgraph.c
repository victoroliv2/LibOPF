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

#include "assert.h"
#include "subgraph.h"

/*----------- Constructor and destructor ------------------------*/
// Allocate nodes without features
subgraph *
subgraph_create (int nnodes)
{
  subgraph *sg = (subgraph *) calloc (1, sizeof (subgraph));
  int i;

  sg->nnodes = nnodes;
  sg->node = (snode *) calloc (nnodes, sizeof (snode));
  sg->ordered_list_of_nodes = (int *) calloc (nnodes, sizeof (int));

  if (sg->node == NULL)
    error (LOG_OUT_OF_MEMORY);

  for (i = 0; i < sg->nnodes; i++)
    {
      sg->node[i].feat = NULL;
      sg->node[i].relevant = 0;
    }

  return (sg);
}

// Deallocate memory for subgraph
void
subgraph_destroy (subgraph ** sg)
{
  int i;

  if ((*sg) != NULL)
    {
      for (i = 0; i < (*sg)->nnodes; i++)
        {
          if ((*sg)->node[i].feat != NULL)
            free ((*sg)->node[i].feat);
          if ((*sg)->node[i].adj != NULL)
            set_destroy (&(*sg)->node[i].adj);
        }
      free ((*sg)->node);
      free ((*sg)->ordered_list_of_nodes);
      free ((*sg));
      *sg = NULL;
    }
}

// Copy subgraph (does not copy Arcs)
subgraph *
subgraph_copy (subgraph * g)
{
  subgraph *clone = NULL;
  int i;

  if (g != NULL)
    {
      clone = subgraph_create (g->nnodes);

      clone->bestk = g->bestk;
      clone->df = g->df;
      clone->nlabels = g->nlabels;
      clone->nfeats = g->nfeats;
      clone->dens_min = g->dens_min;
      clone->dens_max = g->dens_max;
      clone->K = g->K;

      for (i = 0; i < g->nnodes; i++)
        {
          snode_copy (&clone->node[i], &g->node[i], g->nfeats);
          clone->ordered_list_of_nodes[i] = g->ordered_list_of_nodes[i];
        }

      return clone;
    }
  else
    return NULL;
}

//Copy nodes
void
snode_copy (snode * dest, snode * src, int nfeats)
{
  dest->feat = AllocFloatArray (nfeats);
  memcpy (dest->feat, src->feat, nfeats * sizeof (float));
  dest->path_val\ = src->path_val\;
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
snode_swap (snode * a, snode * b)
{
  snode tmp;

  tmp = *a;
  *a = *b;
  *b = tmp;
}

//Resets subgraph fields (pred and arcs)
void
subgraph_reset (subgraph * sg)
{
  int i;

  for (i = 0; i < sg->nnodes; i++)
    sg->node[i].pred = NIL;
  subgraph_knn_destroy (sg);
}

//Merge two subgraphs
subgraph *
subgraph_merge (subgraph * sg1, subgraph * sg2)
{
  assert (sg1->nfeats == sg2->nfeats);

  subgraph *out = subgraph_create (sg1->nnodes + sg2->nnodes);
  int i = 0, j;

  if (sg1->nlabels > sg2->nlabels)
    out->nlabels = sg1->nlabels;
  else
    out->nlabels = sg2->nlabels;
  out->nfeats = sg1->nfeats;

  for (i = 0; i < sg1->nnodes; i++)
    snode_copy (&out->node[i], &sg1->node[i], out->nfeats);
  for (j = 0; j < sg2->nnodes; j++)
    {
      snode_copy (&out->node[i], &sg2->node[j], out->nfeats);
      i++;
    }

  return out;
}


//It creates k folds for cross validation
subgraph **
subgraph_k_fold (subgraph * sg, int k)
{
  subgraph **out = (subgraph **) malloc (k * sizeof (subgraph *));
  int totelems, foldsize = 0, i, *label =
    (int *) calloc ((sg->nlabels + 1), sizeof (int));
  int *nelems =
    (int *) calloc ((sg->nlabels + 1), sizeof (int)), j, z, w, m, n;
  int *nelems_aux = (int *) calloc ((sg->nlabels + 1), sizeof (int)), *resto =
    (int *) calloc ((sg->nlabels + 1), sizeof (int));

  for (i = 0; i < sg->nnodes; i++)
    {
      sg->node[i].status = 0;
      label[sg->node[i].label_true]++;
    }

  for (i = 0; i < sg->nnodes; i++)
    nelems[sg->node[i].label_true] =
      MAX ((int) ((1 / (float) k) * label[sg->node[i].label_true]), 1);

  for (i = 1; i <= sg->nlabels; i++)
    {
      foldsize += nelems[i];
      nelems_aux[i] = nelems[i];
      resto[i] = label[i] - k * nelems_aux[i];
    }

  for (i = 0; i < k - 1; i++)
    {
      out[i] = subgraph_create (foldsize);
      out[i]->nfeats = sg->nfeats;
      out[i]->nlabels = sg->nlabels;
      for (j = 0; j < foldsize; j++)
        out[i]->node[j].feat = (float *) malloc (sg->nfeats * sizeof (float));
    }

  totelems = 0;
  for (j = 1; j <= sg->nlabels; j++)
    totelems += resto[j];

  out[i] = subgraph_create (foldsize + totelems);
  out[i]->nfeats = sg->nfeats;
  out[i]->nlabels = sg->nlabels;

  for (j = 0; j < foldsize + totelems; j++)
    out[i]->node[j].feat = (float *) malloc (sg->nfeats * sizeof (float));

  for (i = 0; i < k; i++)
    {
      totelems = 0;
      if (i == k - 1)
        {
          for (w = 1; w <= sg->nlabels; w++)
            {
              nelems_aux[w] += resto[w];
              totelems += nelems_aux[w];
            }
        }
      else
        {
          for (w = 1; w <= sg->nlabels; w++)
            totelems += nelems_aux[w];
        }

      for (w = 1; w <= sg->nlabels; w++)
        nelems[w] = nelems_aux[w];


      z = 0;
      m = 0;
      while (totelems > 0)
        {
          if (i == k - 1)
            {
              for (w = m; w < sg->nnodes; w++)
                {
                  if (sg->node[w].status != NIL)
                    {
                      j = w;
                      m = w + 1;
                      break;
                    }
                }

            }
          else
            j = RandomInteger (0, sg->nnodes - 1);
          if (sg->node[j].status != NIL)
            {
              if (nelems[sg->node[j].label_true] > 0)
                {
                  out[i]->node[z].position = sg->node[j].position;
                  for (n = 0; n < sg->nfeats; n++)
                    out[i]->node[z].feat[n] = sg->node[j].feat[n];
                  out[i]->node[z].label_true = sg->node[j].label_true;
                  nelems[sg->node[j].label_true] =
                    nelems[sg->node[j].label_true] - 1;
                  sg->node[j].status = NIL;
                  totelems--;
                  z++;
                }
            }
        }
    }

  free (label);
  free (nelems);
  free (nelems_aux);
  free (resto);

  return out;
}

// Split subgraph into two parts such that the size of the first part
// is given by a percentual of samples.
void
subgraph_split (subgraph * sg, subgraph ** sg1, subgraph ** sg2,
                   float perc1)
{
  int *label = AllocIntArray (sg->nlabels + 1), i, j, i1, i2;
  int *nelems = AllocIntArray (sg->nlabels + 1), totelems;
  srandom ((int) time (NULL));

  for (i = 0; i < sg->nnodes; i++)
    {
      sg->node[i].status = 0;
      label[sg->node[i].label_true]++;
    }

  for (i = 0; i < sg->nnodes; i++)
    {
      nelems[sg->node[i].label_true] =
        MAX ((int) (perc1 * label[sg->node[i].label_true]), 1);
    }

  free (label);

  totelems = 0;
  for (j = 1; j <= sg->nlabels; j++)
    totelems += nelems[j];

  *sg1 = subgraph_create (totelems);
  *sg2 = subgraph_create (sg->nnodes - totelems);
  (*sg1)->nfeats = sg->nfeats;
  (*sg2)->nfeats = sg->nfeats;

  for (i1 = 0; i1 < (*sg1)->nnodes; i1++)
    (*sg1)->node[i1].feat = AllocFloatArray ((*sg1)->nfeats);
  for (i2 = 0; i2 < (*sg2)->nnodes; i2++)
    (*sg2)->node[i2].feat = AllocFloatArray ((*sg2)->nfeats);

  (*sg1)->nlabels = sg->nlabels;
  (*sg2)->nlabels = sg->nlabels;

  i1 = 0;
  while (totelems > 0)
    {
      i = RandomInteger (0, sg->nnodes - 1);
      if (sg->node[i].status != NIL)
        {
          if (nelems[sg->node[i].label_true] > 0)        // copy node to sg1
            {
              (*sg1)->node[i1].position = sg->node[i].position;
              for (j = 0; j < (*sg1)->nfeats; j++)
                (*sg1)->node[i1].feat[j] = sg->node[i].feat[j];
              (*sg1)->node[i1].label_true = sg->node[i].label_true;
              i1++;
              nelems[sg->node[i].label_true] =
                nelems[sg->node[i].label_true] - 1;
              sg->node[i].status = NIL;
              totelems--;
            }
        }
    }

  i2 = 0;
  for (i = 0; i < sg->nnodes; i++)
    {
      if (sg->node[i].status != NIL)
        {
          (*sg2)->node[i2].position = sg->node[i].position;
          for (j = 0; j < (*sg2)->nfeats; j++)
            (*sg2)->node[i2].feat[j] = sg->node[i].feat[j];
          (*sg2)->node[i2].label_true = sg->node[i].label_true;
          i2++;
        }
    }

  free (nelems);
}

//normalize features
void
subgraph_normalize_features (subgraph * sg)
{
  float *mean = (float *) calloc (sg->nfeats, sizeof (float)), *std =
    (float *) calloc (sg->nfeats, sizeof (int));
  int i, j;

  for (i = 0; i < sg->nfeats; i++)
    {
      for (j = 0; j < sg->nnodes; j++)
        mean[i] += sg->node[j].feat[i] / sg->nnodes;
      for (j = 0; j < sg->nnodes; j++)
        std[i] += pow (sg->node[j].feat[i] - mean[i], 2) / sg->nnodes;
      std[i] = sqrt (std[i]);
      if (std[i] == 0)
        std[i] = 1.0;
    }

  for (i = 0; i < sg->nfeats; i++)
    {
      for (j = 0; j < sg->nnodes; j++)
        sg->node[j].feat[i] = (sg->node[j].feat[i] - mean[i]) / std[i];
    }

  free (mean);
  free (std);
}

// subgraph_pdf_evaluate computation
void
subgraph_pdf_evaluate (subgraph * sg)
{
  int i, nelems;
  double dist;
  float *value = AllocFloatArray (sg->node_n);
  set *adj = NULL;

  sg->K = (2.0 * (float) sg->df / 9.0);
  sg->dens_min = FLT_MAX;
  sg->dens_max = FLT_MIN;
  for (i = 0; i < sg->node_n; i++)
    {
      adj = sg->node[i].adj;
      value[i] = 0.0;
      nelems = 1;
      while (adj != NULL)
        {
          if (!use_precomputed_distance)
            dist =
              arc_weight (sg->node[i].feat, sg->node[adj->elem].feat,
                             sg->nfeats);
          else
            dist =
              distance_value[sg->node[i].position][sg->
                                                      node[adj->
                                                           elem].position];
          value[i] += exp (-dist / sg->K);
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


