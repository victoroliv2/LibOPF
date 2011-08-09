#ifndef _SUPERVISED_H_
#define _SUPERVISED_H_

#include "common.h"
#include "subgraph.h"

void mst_prototypes (subgraph * sg); /* find prototypes by the mst approach */

/* training function */
void supervised_train (subgraph * sg);

/* classification function: it simply
 * classifies samples from sg */
void supervised_classify (subgraph * sgtrain, float *feat, int sample_n, int *label);

#endif
