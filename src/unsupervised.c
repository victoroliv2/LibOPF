#include "common.h"
#include "set.h"
#include "subgraph.h"
#include "realheap.h"
#include "metrics.h"
#include "knn.h"

#include "unsupervised.h"

static void
remove_plateau_neighbors (struct subgraph * sg)
{
  int i,j;
  struct set *next_adj=NULL;

  for(i = 0; i < sg->node_n; i++)
  {
      /* Eliminating all neighbors that were added
       * for belonging to the plateau of node i.
       * Plateau neighbors are always added to the beginning
       * of the list, since InsertSet works that way, so they
       * must be removed in the same fashion.
       */

      for(j = 0; j < sg->node[i].nplatadj; j++)
      {
          next_adj = sg->node[i].adj->next;
          free(sg->node[i].adj);
          sg->node[i].adj = next_adj;
      }

      sg->node[i].nplatadj = 0;
  }
}

// Estimate the best k by minimum cut
void
opf_best_k_min_cut (struct subgraph * sg, int kmin, int kmax)
{
  int k, k_best = kmax;
  double mincut = DBL_MAX, nc;

  double *maxdists = subgraph_knn_max_distances_evaluate (sg, kmax); // stores the maximum distances for every k=1,2,...,kmax

  // Find the best k
  for (k = kmin; (k <= kmax) && (mincut != 0.0); k++)
    {
      sg->df = maxdists[k - 1];
      sg->k_best = k;

      subgraph_k_max_pdf (sg);

      subgraph_k_max_clustering (sg);

      nc = subgraph_k_max_normalized_cut (sg);

      if (nc < mincut)
        {
          mincut = nc;
          k_best = k;
        }

      remove_plateau_neighbors(sg);
    }
  free (maxdists);
  subgraph_knn_destroy (sg);

  sg->k_best = k_best;

  subgraph_knn_create (sg, sg->k_best);
  subgraph_pdf_evaluate (sg);
}

//Training function: it computes unsupervised training for the
//pre-computed best k.

void
opf_unsupervised_clustering (struct subgraph * sg)
{
  struct set *adj_i, *adj_j;
  char insert_i;
  int i, j;
  int p, q, l;
  double tmp, *path_val = NULL;
  struct real_heap *Q = NULL;
  struct set *Saux = NULL;

  //   Add arcs to guarantee symmetry on plateaus
  for (i = 0; i < sg->node_n; i++)
    {
      adj_i = sg->node[i].adj;
      while (adj_i != NULL)
        {
          j = adj_i->elem;
          if (sg->node[i].dens == sg->node[j].dens)
            {
              // insert i in the adjacency of j if it is not there.
              adj_j = sg->node[j].adj;
              insert_i = 1;
              while (adj_j != NULL)
                {
                  if (i == adj_j->elem)
                    {
                      insert_i = 0;
                      break;
                    }
                  adj_j = adj_j->next;
                }
              if (insert_i)
                set_insert (&(sg->node[j].adj), i);
            }
          adj_i = adj_i->next;
        }
    }

  // Compute clustering

  path_val = alloc_double (sg->node_n);
  Q = real_heap_create (sg->node_n, path_val);
  real_heap_set_removal_policy (Q, REMOVAL_POLICY_MAX);

  for (p = 0; p < sg->node_n; p++)
    {
      path_val[p] = sg->node[p].path_val;
      sg->node[p].pred = NIL;
      sg->node[p].root = p;
      real_heap_insert (Q, p);
    }

  l = 0;
  i = 0;
  while (!real_heap_is_empty (Q))
    {
      real_heap_remove (Q, &p);
      sg->ordered_list_of_nodes[i] = p;
      i++;

      if (sg->node[p].pred == NIL)
        {
          path_val[p] = sg->node[p].dens;
          sg->node[p].label = l;
          l++;
        }

      sg->node[p].path_val = path_val[p];
      for (Saux = sg->node[p].adj; Saux != NULL; Saux = Saux->next)
        {
          q = Saux->elem;
          if (Q->color[q] != COLOR_BLACK)
            {
              tmp = MIN (path_val[p], sg->node[q].dens);
              if (tmp > path_val[q])
                {
                  real_heap_update (Q, q, tmp);
                  sg->node[q].pred = p;
                  sg->node[q].root = sg->node[p].root;
                  sg->node[q].label = sg->node[p].label;
                }
            }
        }
    }

  sg->label_n = l;

  real_heap_destroy (&Q);
  free (path_val);
}

// Classification function ----- it classifies nodes of sg by using
// the OPF-clustering labels from sg_train

void
opf_unsupervised_knn_classify (struct subgraph * sg_train, double *feat, int sample_n, int *label)
{
  int i, j;
  double weight;

  for (i = 0; i < sample_n; i++)
    {
      for (j = 0; (j < sg_train->node_n); j++)
        {
          int l;
          l = sg_train->ordered_list_of_nodes[j];

          weight = sg_train->arc_weight
                     (sg_train->node[l].feat, &feat[i*sg_train->feat_n], sg_train->feat_n);

          if (weight <= sg_train->node[l].radius)
            {
              label[i] = sg_train->node[l].label;
              break;
            }
        }
    }

}
