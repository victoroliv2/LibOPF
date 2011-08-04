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
Subgraph *
CreateSubgraph (int nnodes)
{
  Subgraph *sg = (Subgraph *) calloc (1, sizeof (Subgraph));
  int i;

  sg->nnodes = nnodes;
  sg->node = (SNode *) calloc (nnodes, sizeof (SNode));
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
DestroySubgraph (Subgraph ** sg)
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
Subgraph *
CopySubgraph (Subgraph * g)
{
  Subgraph *clone = NULL;
  int i;

  if (g != NULL)
    {
      clone = CreateSubgraph (g->nnodes);

      clone->bestk = g->bestk;
      clone->df = g->df;
      clone->nlabels = g->nlabels;
      clone->nfeats = g->nfeats;
      clone->mindens = g->mindens;
      clone->maxdens = g->maxdens;
      clone->K = g->K;

      for (i = 0; i < g->nnodes; i++)
        {
          CopySNode (&clone->node[i], &g->node[i], g->nfeats);
          clone->ordered_list_of_nodes[i] = g->ordered_list_of_nodes[i];
        }

      return clone;
    }
  else
    return NULL;
}

//Copy nodes
void
CopySNode (SNode * dest, SNode * src, int nfeats)
{
  dest->feat = AllocFloatArray (nfeats);
  memcpy (dest->feat, src->feat, nfeats * sizeof (float));
  dest->pathval = src->pathval;
  dest->dens = src->dens;
  dest->label = src->label;
  dest->root = src->root;
  dest->pred = src->pred;
  dest->truelabel = src->truelabel;
  dest->position = src->position;
  dest->status = src->status;
  dest->relevant = src->relevant;
  dest->radius = src->radius;
  dest->nplatadj = src->nplatadj;

  dest->adj = set_clone (src->adj);
}


//Swap nodes
void
SwapSNode (SNode * a, SNode * b)
{
  SNode tmp;

  tmp = *a;
  *a = *b;
  *b = tmp;
}

void
opf_ResetSubgraph (Subgraph * sg)
{
  int i;

  for (i = 0; i < sg->nnodes; i++)
    sg->node[i].pred = NIL;
  opf_DestroyArcs (sg);
}

//Merge two subgraphs
Subgraph *
opf_MergeSubgraph (Subgraph * sg1, Subgraph * sg2)
{
  assert (sg1->nfeats == sg2->nfeats);

  Subgraph *out = CreateSubgraph (sg1->nnodes + sg2->nnodes);
  int i = 0, j;

  if (sg1->nlabels > sg2->nlabels)
    out->nlabels = sg1->nlabels;
  else
    out->nlabels = sg2->nlabels;
  out->nfeats = sg1->nfeats;

  for (i = 0; i < sg1->nnodes; i++)
    CopySNode (&out->node[i], &sg1->node[i], out->nfeats);
  for (j = 0; j < sg2->nnodes; j++)
    {
      CopySNode (&out->node[i], &sg2->node[j], out->nfeats);
      i++;
    }

  return out;
}
