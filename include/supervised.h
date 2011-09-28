#ifndef _SUPERVISED_H_
#define _SUPERVISED_H_

#include "common.h"
#include "graph.h"

/* training function */
void opf_supervised_train (struct opf_graph * sg);

void opf_supervised_train_iterative (struct opf_graph *sg, double split);

void opf_supervised_train_agglomerative (struct opf_graph *sg, double split);

/* classification function: it simply
 * classifies samples from sg */
void opf_supervised_classify (struct opf_graph * sgtrain, double *feat, int sample_n, int *label);

#endif
