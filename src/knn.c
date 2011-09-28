#include "common.h"
#include "metrics.h"
#include "realheap.h"
#include "knn.h"

// Create adjacent list in opf_graph: a knn graph
void
opf_graph_knn_create (struct opf_graph * sg, int knn)
{
  int i, j, l, k;
  double dist;
  int *nn = alloc_int (knn + 1);
  double *d = alloc_double (knn + 1);

  /* Create graph with the knn-nearest neighbors */

  sg->df = 0.0;
  for (i = 0; i < sg->node_n; i++)
    {
      for (l = 0; l < knn; l++)
        d[l] = DBL_MAX;
      for (j = 0; j < sg->node_n; j++)
        {
          if (j != i)
            {
              d[knn] = opf_graph_get_distance (sg, &sg->node[i], &sg->node[j]);

              nn[knn] = j;
              k = knn;
              while ((k > 0) && (d[k] < d[k - 1]))
                {
                  dist = d[k];
                  l = nn[k];
                  d[k] = d[k - 1];
                  nn[k] = nn[k - 1];
                  d[k - 1] = dist;
                  nn[k - 1] = l;
                  k--;
                }
            }
        }

      for (l = 0; l < knn; l++)
        {
          if (d[l] != INT_MAX)
            {
              if (d[l] > sg->df)
                sg->df = d[l];
              //if (d[l] > sg->node[i].radius)
              sg->node[i].radius = d[l];
              set_insert (&(sg->node[i].adj), nn[l]);
            }
        }
    }
  free (d);
  free (nn);

  if (sg->df < 0.00001)
    sg->df = 1.0;
}

// Returns an array with the maximum distances
// for each k=1,2,...,kmax
double *
opf_graph_knn_max_distances_evaluate (struct opf_graph * sg, int kmax)
{
  int i, j, l, k;
  double dist;
  int *nn = alloc_int (kmax + 1);
  double *d = alloc_double (kmax + 1);
  double *maxdists = alloc_double (kmax);
  /* Create graph with the knn-nearest neighbors */

  sg->df = 0.0;
  for (i = 0; i < sg->node_n; i++)
    {
      for (l = 0; l < kmax; l++)
        d[l] = DBL_MAX;
      for (j = 0; j < sg->node_n; j++)
        {
          if (j != i)
            {
              d[kmax] = opf_graph_get_distance (sg, &sg->node[i], &sg->node[j]);

              nn[kmax] = j;
              k = kmax;
              while ((k > 0) && (d[k] < d[k - 1]))
                {
                  dist = d[k];
                  l = nn[k];
                  d[k] = d[k - 1];
                  nn[k] = nn[k - 1];
                  d[k - 1] = dist;
                  nn[k - 1] = l;
                  k--;
                }
            }
        }
      sg->node[i].radius = 0.0;
      sg->node[i].nplatadj = 0; //zeroing amount of nodes on plateaus
      //making sure that the adjacent nodes be sorted in non-decreasing order
      for (l = kmax - 1; l >= 0; l--)
        {
          if (d[l] != DBL_MAX)
            {
              if (d[l] > sg->df)
                sg->df = d[l];
              if (d[l] > sg->node[i].radius)
                sg->node[i].radius = d[l];
              if (d[l] > maxdists[l])
                maxdists[l] = d[l];
              //adding the current neighbor at the beginnig of the list
              set_insert (&(sg->node[i].adj), nn[l]);
            }
        }
    }
  free (d);
  free (nn);

  if (sg->df < 0.00001)
    sg->df = 1.0;

  return maxdists;
}

// Destroy Arcs
void
opf_graph_knn_destroy (struct opf_graph * sg)
{
  int i;

  for (i = 0; i < sg->node_n; i++)
    {
        sg->node[i].nplatadj = 0;
        set_destroy (&(sg->node[i].adj));
    }
}

// OPFClustering computation only for sg->k_best neighbors
void
opf_graph_k_max_clustering (struct opf_graph * sg)
{
  struct set *adj_i, *adj_j;
  char insert_i;
  int i, j;
  int p, q, l, ki, kj;
  const int kmax = sg->k_best;
  double tmp, *path_val = NULL;
  struct real_heap *Q = NULL;
  struct set *Saux = NULL;

  //   Add arcs to guarantee symmetry on plateaus
  for (i = 0; i < sg->node_n; i++)
    {
      adj_i = sg->node[i].adj;
      ki = 1;
      while (ki <= kmax)
        {
          j = adj_i->elem;
          if (sg->node[i].dens == sg->node[j].dens)
            {
              // insert i in the adjacency of j if it is not there.
              adj_j = sg->node[j].adj;
              insert_i = 1;
              kj = 1;
              while (kj <= kmax)
                {
                  if (i == adj_j->elem)
                    {
                      insert_i = 0;
                      break;
                    }
                  adj_j = adj_j->next;
                  kj++;
                }
              if (insert_i)
                {
                  set_insert (&(sg->node[j].adj), i);
                  sg->node[j].nplatadj++;       //number of adjacent nodes on
                  //plateaus (includes adjacent plateau
                  //nodes computed for previous kmax's)
                }
            }
          adj_i = adj_i->next;
          ki++;
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
      const int nadj = sg->node[p].nplatadj + kmax;     // total amount of neighbors
      for (Saux = sg->node[p].adj, ki = 1; ki <= nadj;
           Saux = Saux->next, ki++)
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


// PDF computation only for sg->k_best neighbors
void
opf_graph_k_max_pdf (struct opf_graph * sg)
{
  int i, nelems;
  const int kmax = sg->k_best;
  double dist;
  double *value = alloc_double (sg->node_n);
  struct set *adj = NULL;

  sg->k = (2.0 * (double) sg->df / 9.0);

  sg->dens_min = DBL_MAX;
  sg->dens_max = DBL_MIN;
  for (i = 0; i < sg->node_n; i++)
    {
      adj = sg->node[i].adj;
      value[i] = 0.0;
      nelems = 1;
      int k;
      //the PDF is computed only for the kmax adjacents
      //because it is assumed that there will be no plateau
      //neighbors yet, i.e. nplatadj = 0 for every node in sg
      for (k = 1; k <= kmax; k++)
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

// Normalized cut computed only for sg->k_best neighbors
double
opf_graph_k_max_normalized_cut (struct opf_graph * sg)
{
  int l, p, q, k;
  const int kmax = sg->k_best;
  struct set *Saux;
  double ncut, dist;
  double *acumIC;                //acumulate weights inside each class
  double *acumEC;                //acumulate weights between the class and a distinct one

  ncut = 0.0;
  acumIC = alloc_double (sg->label_n);
  acumEC = alloc_double (sg->label_n);

  for (p = 0; p < sg->node_n; p++)
    {
      const int nadj = sg->node[p].nplatadj + kmax;     //for plateaus the number of adjacent
      //nodes will be greater than the current
      //kmax, but they should be considered
      for (Saux = sg->node[p].adj, k = 1; k <= nadj; Saux = Saux->next, k++)
        {
          q = Saux->elem;

          dist = opf_graph_get_distance (sg, &sg->node[p], &sg->node[q]);

          if (dist > 0.0)
            {
              if (sg->node[p].label == sg->node[q].label)
                {
                  acumIC[sg->node[p].label] += 1.0 / dist;      // intra-class weight
                }
              else              // inter - class weight
                {
                  acumEC[sg->node[p].label] += 1.0 / dist;      // inter-class weight
                }
            }
        }
    }

  for (l = 0; l < sg->label_n; l++)
    {
      if (acumIC[l] + acumEC[l] > 0.0)
        ncut += (double) acumEC[l] / (acumIC[l] + acumEC[l]);
    }
  free (acumEC);
  free (acumIC);
  return (ncut);
}
