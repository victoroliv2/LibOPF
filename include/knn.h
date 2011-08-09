#ifndef __KNN_H__
#define __KNN_H__

#include "subgraph.h"

void   subgraph_knn_create                 (struct subgraph * sg, int knn);            /* it creates arcs for each node (adjacency relation)           */
void   subgraph_knn_destroy                (struct subgraph * sg);                     /* it destroys the adjacency relation                           */
float *subgraph_knn_max_distances_evaluate (struct subgraph * sg, int kmax);           /* creates arcs for each node (adjacency relation) and returns
                                                                                   the maximum distances for each k=1,2,...,kmax                */

void   subgraph_k_max_pdf                  (struct subgraph * sg);                     /* pdf computation only for sg->bestk neighbors                 */
void   subgraph_k_max_clustering           (struct subgraph * sg);                     /* opfclustering computation only for sg->bestk neighbors       */
float  subgraph_k_max_normalized_cut       (struct subgraph * sg);                     /* normalized cut computed only for sg->bestk neighbors         */
void   subgraph_best_k_min_cut             (struct subgraph * sg, int kmin, int kmax);

#endif
