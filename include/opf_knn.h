void       createarcs                (subgraph * sg, int knn);                          /* it creates arcs for each node (adjacency relation)                      */
float     *createarcs2               (subgraph * sg, int kmax);                         /* creates arcs for each node (adjacency relation) and returns 
																																												   the maximum distances for each k=1,2,...,kmax                            */
void       destroyarcs               (subgraph * sg);                                   /* it destroys the adjacency relation                                      */
void       pdf                       (subgraph * sg);                                   /* it computes the pdf for each node                                       */
void       pdftokmax                 (subgraph * sg);                                   /* pdf computation only for sg->bestk neighbors                             */
void       bestkmincut               (subgraph * sg, int kmin, int kmax);
float      normalizedcut             (subgraph * sg);              
void       opfclusteringtokmax       (subgraph * sg);                                   /* opfclustering computation only for sg->bestk neighbors                   */
float      normalizedcuttokmax       (subgraph * sg);                                   /* normalized cut computed only for sg->bestk neighbors                     */
