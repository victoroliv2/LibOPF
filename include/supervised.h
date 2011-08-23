#ifndef _SUPERVISED_H_
#define _SUPERVISED_H_

#include "common.h"
#include "subgraph.h"

void mst_prototypes (struct subgraph * sg); /* find prototypes by the mst approach */

/* training function */
void supervised_train (struct subgraph * sg);

void supervised_train_iterative (struct subgraph *sg,
                                 float *eval_feat, int *eval_label, int eval_n);

void supervised_train_agglomerative (struct subgraph *sg,
                                     float *eval_feat, int *eval_label, int eval_n);

/* classification function: it simply
 * classifies samples from sg */
void supervised_classify (struct subgraph * sgtrain, float *feat, int sample_n, int *label);

#endif
