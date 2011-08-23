# includes

cdef extern from "subgraph.h":
    cdef enum METRIC:
      EUCLIDIAN          = 0
      LOG_EUCLIDIAN      = 1
      CHI_SQUARE         = 2
      MANHATTAN          = 3
      CANBERRA           = 4
      SQUARED_CHORD      = 5
      SQUARED_CHI_SQUARE = 6
      BRAY_CURTIS        = 7
      NO_METRIC          = 8

    cdef struct subgraph:
      pass

    subgraph * subgraph_create       (int node_n)
    void       subgraph_destroy      (subgraph ** sg)
    void       subgraph_set_metric   (subgraph *sg, METRIC m)
    bint       subgraph_set_data     (subgraph *sg, float *feat, int *label, int feat_n)
    void       subgraph_precompute_distance (subgraph *sg,
                                             float (*arc_weight) (float *f1, float *f2, int n),
                                             METRIC m)

cdef extern from "supervised.h":
    void supervised_train    (subgraph * sg)
    void supervised_train_iterative (subgraph *sg,
                                     float *eval_feat, int *eval_label, int eval_n)
    void supervised_train_agglomerative (subgraph *sg,
                                         float *eval_feat, int *eval_label, int eval_n)
    void supervised_classify (subgraph * sgtrain, float *feat, int sample_n, int *label)
