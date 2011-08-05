void       createarcs                (subgraph * sg, int knn);                          /* it creates arcs for each node (adjacency relation)                      */
void       destroyarcs               (subgraph * sg);                                   /* it destroys the adjacency relation                                      */
void       pdf                       (subgraph * sg);                                   /* it computes the pdf for each node                                       */
void       pdftokmax                 (subgraph * sg);                                   /* pdf computation only for sg->bestk neighbors                             */
