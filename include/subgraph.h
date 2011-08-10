#ifndef _SUBGRAPH_H_
#define _SUBGRAPH_H_

#include "common.h"
#include "set.h"
#include "metrics.h"

enum STATUS
{
  STATUS_NOTHING   = 0,
  STATUS_PROTOTYPE = 1
};

enum RELEVT
{
  RELEVT_NOT = 0,
  RELEVT_IS  = 1
};

struct snode
{
  float   path_val;             /* path value                                           */
  float   dens;                 /* node density                                         */
  float   radius;               /* maximum distance among the k-nearest neighbors in
                                 * the training set. it is used to propagate
                                 * clustering labels to testing nodes)                  */
  int     label;                /* node label                                           */
  int     root;                 /* root node                                            */
  int     pred;                 /* predecessor node                                     */
  int     label_true;           /* true label if it is known                            */
  int     position;             /* index in the feature space                           */
  float  *feat;                 /* feature vector                                       */
  enum STATUS  status;          /* 0 - nothing, 1 - prototype                           */
  enum RELEVT  relevant;        /* 0 - irrelevant, 1 - relevant                         */

  int     nplatadj;             /* holds the amount of adjacent nodes on plateaus
                                 * it is used to optimize opf_bestkmincut
                                 * because a knn graph need only be built
                                 * for kmax, but the opf_pdf and opf_normalizedcut
                                 * computation need only be done for the current k,
                                 * or until k+nplatadj is reached
                                 */

  struct set *adj;               /* adjacency list for knn graphs */
};

void snode_copy (struct snode * dest, struct snode * src, int feat_n); /* copy nodes */
void snode_swap (struct snode * a, struct snode * b);                  /* swap nodes */


enum METRIC
{
  EUCLIDIAN          = 0,
  LOG_EUCLIDIAN      = 1,
  /*GAUSSIAN           = 2,*/
  CHI_SQUARE         = 2,
  MANHATTAN          = 3,
  CANBERRA           = 4,
  SQUARED_CHORD      = 5,
  SQUARED_CHI_SQUARE = 6,
  BRAY_CURTIS        = 7
};

struct subgraph
{
  struct snode  *node;           /* nodes of the image/scene subgraph                   */
  int     node_n;                /* number of nodes                                     */
  int     feat_n;                /* number of features                                  */
  int     k_best;                /* number of adjacent nodes                            */
  int     label_n;               /* number of clusters                                  */
  float   df;                    /* radius in the feature space for density computation */
  float   dens_min;              /* minimum density value                               */
  float   dens_max;              /* maximum density value                               */
  float   k;                     /* constant for opf_pdf computation                    */
  int    *ordered_list_of_nodes; /* store the list of nodes in the increasing order
                                  * of cost for speeding up supervised classification.
                                  */
  float *feat_data; /* memory pointer to all features data */
  int use_precomputed_distance;
  float (*arc_weight) (float *f1, float *f2, int n);
  float **distance_value;
};

struct subgraph * subgraph_create       (int node_n);     /* allocates nodes without features        */
void              subgraph_destroy      (struct subgraph ** sg); /* deallocates memory for subgraph         */
int               subgraph_set_data     (struct subgraph *sg, float *feat, int *label, int node_n, int feat_n);
void              subgraph_set_metric   (struct subgraph *sg, enum METRIC m);
void              subgraph_pdf_evaluate (struct subgraph * sg);
#endif /* _SUBGRAPH_H_ */
