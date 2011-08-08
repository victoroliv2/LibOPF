#ifndef _SUPERVISED_H_
#define _SUPERVISED_H_

#include "common.h"
#include "subgraph.h"

void       swap_errors_by_non_prototypes (subgraph ** sg_train, subgraph ** sg_eval);         /* replace errors from evaluating set by non prototypes from training set  */
void       move_irrelevant_nodes         (subgraph ** sgtrain, subgraph ** sgeval);          /* move irrelevant nodes from source graph (src) to destiny graph (dst)    */
void       move_misclassified_nodes      (subgraph ** sgtrain, subgraph ** sgeval, int *p);  /* move misclassified nodes from source graph (src) to destiny graph (dst) */
void       remove_irrelevant_nodes       (subgraph ** sg);                                   /* remove irrelevant nodes                                                 */
void       mark_nodes                    (subgraph * g, int i);                              /* mark nodes and the whole path as relevants                              */
void       mst_prototypes                (subgraph * sg);                                    /* find prototypes by the mst approach                                     */

/* training function */
void supervised_train (subgraph * sg);

/* classification function: it simply
 * classifies samples from sg */
void supervised_classify (subgraph * sgtrain, subgraph * sg);

/* learning function */
void supervised_learn (subgraph ** sgtrain, subgraph ** sgeval);

/* agglomerative learning function */
void supervised_agglomerative_learn (subgraph ** sgtrain, subgraph ** sgeval);

#endif
