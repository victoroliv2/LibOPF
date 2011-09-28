#ifndef __KNN_H__
#define __KNN_H__

#include "graph.h"

void   opf_graph_knn_create                 (struct opf_graph * sg, int knn);            /* it creates arcs for each node (adjacency relation)           */
void   opf_graph_knn_destroy                (struct opf_graph * sg);                     /* it destroys the adjacency relation                           */
double *opf_graph_knn_max_distances_evaluate (struct opf_graph * sg, int kmax);           /* creates arcs for each node (adjacency relation) and returns
                                                                                   the maximum distances for each k=1,2,...,kmax                */

void   opf_graph_k_max_pdf                  (struct opf_graph * sg);                     /* pdf computation only for sg->bestk neighbors                 */
void   opf_graph_k_max_clustering           (struct opf_graph * sg);                     /* opfclustering computation only for sg->bestk neighbors       */
double  opf_graph_k_max_normalized_cut       (struct opf_graph * sg);                     /* normalized cut computed only for sg->bestk neighbors         */

#endif
