#ifndef _SUBGRAPH_H_
#define _SUBGRAPH_H_

#include "common.h"
#include "set.h"
#include "metrics.h"

typedef enum
{
  STATUS_NOTHING   = 0,
  STATUS_PROTOTYPE = 1
} STATUS;

typedef enum
{
  RELEVT_NOT = 0,
  RELEVT_IS  = 1
} RELEVT;

typedef struct
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
  STATUS  status;               /* 0 - nothing, 1 - prototype                           */
  RELEVT  relevant;             /* 0 - irrelevant, 1 - relevant                         */

  int     nplatadj;             /* holds the amount of adjacent nodes on plateaus
                                 * it is used to optimize opf_bestkmincut
                                 * because a knn graph need only be built
                                 * for kmax, but the opf_pdf and opf_normalizedcut
                                 * computation need only be done for the current k,
                                 * or until k+nplatadj is reached
                                 */

  set    *adj;                  /* adjacency list for knn graphs */
} snode;

void snode_copy (snode * dest, snode * src, int feat_n); /* copy nodes */
void snode_swap (snode * a, snode * b);                  /* swap nodes */


typedef enum
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
} METRIC;

typedef struct
{
  snode  *node;                  /* nodes of the image/scene subgraph                   */
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

  int use_precomputed_distance;
  arc_weight_function arc_weight;
  float **distance_value;
} subgraph;

subgraph * subgraph_create       (int node_n);     /* allocates nodes without features        */
void       subgraph_destroy      (subgraph ** sg); /* deallocates memory for subgraph         */
void       subgraph_set_metric   (subgraph *sg, METRIC m);
void       subgraph_pdf_evaluate (subgraph * sg);
#endif /* _SUBGRAPH_H_ */
