#ifndef _OPF_H_
#define _OPF_H_

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

#include "common.h"
#include "set.h"
#include "subgraph.h"
#include "sgctree.h"
#include "realheap.h"
#include "metrics.h"

#define ARCW_MAX 100000.0
#define DENS_MAX 1000.0

void       resetsubgraph             (subgraph * sg);                                   /* resets subgraph fields (pred and arcs)                                  */
void       swaperrorsbynonprototypes (subgraph ** sgtrain, subgraph ** sgeval);         /* replace errors from evaluating set by non prototypes from training set  */
void       moveirrelevantnodes       (subgraph ** sgtrain, subgraph ** sgeval);         /* move irrelevant nodes from source graph (src) to destiny graph (dst)    */
void       movemisclassifiednodes    (subgraph ** sgtrain, subgraph ** sgeval, int *p); /* move misclassified nodes from source graph (src) to destiny graph (dst) */
void       removeirrelevantnodes     (subgraph ** sg);                                  /* remove irrelevant nodes                                                 */
void       marknodes                 (subgraph * g, int i);                             /* mark nodes and the whole path as relevants                              */
void       writemodelfile            (subgraph * g, char *file);                        /* write model file to disk                                                */
subgraph * readmodelfile             (char *file);                                      /* read subgraph from opf model file                                       */
void       normalizefeatures         (subgraph * sg);                                   /* normalize features                                                      */
void       mstprototypes             (subgraph * sg);                                   /* find prototypes by the mst approach                                     */
subgraph **kfoldsubgraph             (subgraph * sg, int k);                            /* it creates k folds for cross validation                                 */
void       splitsubgraph             (subgraph * sg, subgraph ** sg1, subgraph ** sg2,
                                                                          float perc1); /* split subgraph into two parts such that the size of the  first part is
                                                                                         * given by a percentual of samples.                                       */
subgraph  *mergesubgraph             (subgraph * sg1, subgraph * sg2);                  /* merge two subgraphs                                                     */
float      accuracy                  (subgraph * g);                                    /* compute accuracy                                                        */
int      **confusionmatrix           (subgraph * sg);                                   /* compute the confusion matrix                                            */
float    **readdistances             (char *filename, int *n);                          /* read distances from precomputed distances file                          */
float      normalizedcut             (subgraph * sg);
void       bestkmincut               (subgraph * sg, int kmin, int kmax);
void       createarcs                (subgraph * sg, int knn);                          /* it creates arcs for each node (adjacency relation)                      */
void       destroyarcs               (subgraph * sg);                                   /* it destroys the adjacency relation                                      */
void       pdf                       (subgraph * sg);                                   /* it computes the pdf for each node                                       */
void       elimmaxbelowvolume        (subgraph * sg, int v);                            /* eliminate maxima in the graph with volume below v                       */
void       elimmaxbelowarea          (subgraph * sg, int a);                            /* eliminate maxima in the graph with area below a                         */
void       elimmaxbelowh             (subgraph * sg, float h);                          /* eliminate maxima in the graph with pdf below h                          */

/* -------- auxiliary functions used to optimize bestkmincut -------- */
float     *opf_createarcs2               (subgraph * sg, int kmax);                     /* creates arcs for each node (adjacency relation) and returns
                                                                                         * the maximum distances for each k=1,2,...,kmax                            */
void       opf_opfclusteringtokmax       (subgraph * sg);                               /* opfclustering computation only for sg->bestk neighbors                   */
void       opf_pdftokmax                 (subgraph * sg);                               /* pdf computation only for sg->bestk neighbors                             */
float      opf_normalizedcuttokmax       (subgraph * sg);                               /* normalized cut computed only for sg->bestk neighbors                     */
#endif
