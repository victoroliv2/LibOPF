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

    cdef struct subgraph:
      pass

    subgraph * subgraph_create       (int node_n)
    void       subgraph_destroy      (subgraph ** sg)
    void       subgraph_set_metric   (subgraph *sg, METRIC m)
    bint       subgraph_set_data     (subgraph *sg, float *feat, int *label, int node_n, int feat_n)

cdef extern from "supervised.h":
    void supervised_train    (subgraph * sg)
    void supervised_classify (subgraph * sgtrain, float *feat, int sample_n, int *label)
