#ifndef __UNSUPERVISED_H__
#define __UNSUPERVISED_H__

#include "subgraph.h"

/* Training function: it computes unsupervised
 * training for the pre-computed best k. */
void unsupervised_clustering (subgraph * sg);

/* classification function: it classifies nodes
 * of sg by using the labels created by program
 * opf_cluster in sgtrain */
void unsupervised_knn_classify (subgraph * sgtrain, subgraph * sg);

#endif
