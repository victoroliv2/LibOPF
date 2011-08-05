#ifndef _MEASURES_H_
#define _MEASURES_H_

#include "subgraph.h"

float   subgraph_accuracy         (subgraph * sg); /* compute accuracy             */
int   **subgraph_confusion_matrix (subgraph * sg); /* compute the confusion matrix */
float   subgraph_normalized_cut   (subgraph * sg);

#endif
