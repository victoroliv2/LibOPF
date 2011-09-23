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

  int     nplatadj;             /* holds the amount of adjacent nodes on plateaus
                                 * it is used to optimize opf_bestkmincut
                                 * because a knn graph need only be built
                                 * for kmax, but the opf_pdf and opf_normalizedcut
                                 * computation need only be done for the current k,
                                 * or until k+nplatadj is reached
                                 */

  struct set *adj;               /* adjacency list for knn graphs */
};

void snode_clear (struct snode *n);
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
  BRAY_CURTIS        = 7,
  NO_METRIC          = 8
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
  float (*arc_weight) (float *f1, float *f2, int n);

  /* precomputed distance matrix */
  float *pdist;
  int pdist_train_stride; /* stride of training (pdist) distance matrix */
};

#define PDISTANCE(sg,i,j) ( (sg)->pdist[i*(sg)->pdist_train_stride +j] )

struct subgraph * subgraph_create       (int node_n);     /* allocates nodes without features        */
void              subgraph_destroy      (struct subgraph ** sg); /* deallocates memory for subgraph         */
int               subgraph_set_feature  (struct subgraph *sg, float *feat, int *label, int feat_n);
void              subgraph_set_metric   (struct subgraph *sg, float (*arc_weight) (float *f1, float *f2, int n), enum METRIC m);

int               subgraph_set_precomputed_distance
                                        (struct subgraph *sg, float *dist, int *label);

void              subgraph_pdf_evaluate (struct subgraph * sg);

void              subgraph_resize       (struct subgraph * sg, int node_n);

inline static
float subgraph_get_distance (struct subgraph * sg, struct snode *i, struct snode *j)
{
  if (sg->pdist)
    return PDISTANCE(sg, i->position, j->position);
  else
    return sg->arc_weight(i->feat, j->feat, sg->feat_n);
}

#endif /* _SUBGRAPH_H_ */
