#ifndef __UNSUPERVISED_H__
#define __UNSUPERVISED_H__

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

/* Training function: it computes unsupervised
 * training for the pre-computed best k. */
void unsupervised_clustering (subgraph * sg);

/* classification function: it classifies nodes
 * of sg by using the labels created by program
 * opf_cluster in sgtrain */
void unsupervised_knn_classify (subgraph * sgtrain, subgraph * sg);

#endif
