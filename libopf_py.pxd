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

    void       subgraph_set_metric   (subgraph *sg,
                                      double (*arc_weight) (double *f1, double *f2, int n),
                                      METRIC m)

    bint       subgraph_set_feature  (subgraph *sg, double *feat, int *label, int feat_n)

    bint       subgraph_set_precomputed_distance (subgraph *sg,
                                                  double *distance,
                                                  int *label)

    void       subgraph_pdf_evaluate (subgraph * sg)

cdef extern from "supervised.h":
    void opf_supervised_train    (subgraph * sg)
    void opf_supervised_train_iterative (subgraph *sg, double split)
    void opf_supervised_train_agglomerative (subgraph *sg, double split)
    void opf_supervised_classify (subgraph * sgtrain, double *feat, int sample_n, int *label)

cdef extern from "unsupervised.h":
    void opf_best_k_min_cut (subgraph * sg, int kmin, int kmax)
    void opf_unsupervised_clustering (subgraph * sg)
    void opf_unsupervised_knn_classify (subgraph * sgtrain, double *feat, int sample_n, int *label)
