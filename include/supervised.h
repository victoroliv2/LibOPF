#ifndef _SUPERVISED_H_
#define _SUPERVISED_H_

#include "common.h"
#include "subgraph.h"

/* training function */
void opf_supervised_train (struct subgraph * sg);

void opf_supervised_train_iterative (struct subgraph *sg,
                                     float *eval_feat, int *eval_label, int eval_n);

void opf_supervised_train_agglomerative (struct subgraph *sg,
                                         float *eval_feat, int *eval_label, int eval_n);

/* classification function: it simply
 * classifies samples from sg */
void opf_supervised_classify (struct subgraph * sgtrain, float *feat, int sample_n, int *label);

#endif
