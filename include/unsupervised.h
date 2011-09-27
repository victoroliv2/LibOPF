#ifndef __UNSUPERVISED_H__
#define __UNSUPERVISED_H__

#include "subgraph.h"

void opf_best_k_min_cut (struct subgraph * sg, int kmin, int kmax);

/* Training function: it computes unsupervised
 * training for the pre-computed best k. */
void opf_unsupervised_clustering (struct subgraph * sg);

/* classification function: it classifies nodes
 * of sg by using the labels created by program
 * opf_cluster in sgtrain */
void opf_unsupervised_knn_classify (struct subgraph * sgtrain, double *feat, int sample_n, int *label);

#endif
