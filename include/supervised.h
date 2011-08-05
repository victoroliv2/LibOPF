#ifndef _OPF_SUPERVISED_H_
#define _OPF_SUPERVISED_H_

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

void       swap_errors_by_non_prototypes (subgraph ** sg_train, subgraph ** sg_eval);         /* replace errors from evaluating set by non prototypes from training set  */
void       move_irrelevant_nodes         (subgraph ** sgtrain, subgraph ** sgeval);          /* move irrelevant nodes from source graph (src) to destiny graph (dst)    */
void       move_misclassified_nodes      (subgraph ** sgtrain, subgraph ** sgeval, int *p);  /* move misclassified nodes from source graph (src) to destiny graph (dst) */
void       remove_irrelevant_nodes       (subgraph ** sg);                                   /* remove irrelevant nodes                                                 */
void       mark_nodes                    (subgraph * g, int i);                              /* mark nodes and the whole path as relevants                              */
void       mst_prototypes                (subgraph * sg);                                    /* find prototypes by the mst approach                                     */

/* training function */
void supervised_training (subgraph * sg);

/* classification function: it simply
 * classifies samples from sg */
void supervised_classifying (subgraph * sgtrain, subgraph * sg);

/* learning function */
void supervised_learning (subgraph ** sgtrain, subgraph ** sgeval);

/* agglomerative learning function */
void supervised_agglomerative_learning (subgraph ** sgtrain, subgraph ** sgeval);

#endif
