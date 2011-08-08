#include "common.h"
#include "set.h"
#include "subgraph.h"
#include "realheap.h"
#include "metrics.h"

#include "unsupervised.h"

//Training function: it computes unsupervised training for the
//pre-computed best k.

void
unsupervised_clustering (subgraph * sg)
{
  set *adj_i, *adj_j;
  char insert_i;
  int i, j;
  int p, q, l;
  float tmp, *path_val = NULL;
  real_heap *Q = NULL;
  set *Saux = NULL;

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

  path_val = alloc_float (sg->node_n);
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
unsupervised_knn_classify (subgraph * sg_train, subgraph * sg)
{
  int i, j, k;
  float weight;

  for (i = 0; i < sg->node_n; i++)
    {
      for (j = 0; (j < sg_train->node_n); j++)
        {
          k = sg_train->ordered_list_of_nodes[j];
          if (!use_precomputed_distance)
            weight =
              arc_weight (sg_train->node[k].feat, sg->node[i].feat,
                             sg->feat_n);
          else
            weight =
              distance_value[sg_train->node[k].position][sg->
                                                           node[i].position];
          if (weight <= sg_train->node[k].radius)
            {
              sg->node[i].label = sg_train->node[k].label;
              break;
            }
        }
    }

}
