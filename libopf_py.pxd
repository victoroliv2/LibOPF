# includes

cdef extern from "graph.h":
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

    cdef struct opf_graph:
      pass

    opf_graph * opf_graph_create       (int node_n)
    void       opf_graph_destroy      (opf_graph ** sg)

    void       opf_graph_set_metric   (opf_graph *sg,
                                      double (*arc_weight) (double *f1, double *f2, int n),
                                      METRIC m)

    bint       opf_graph_set_feature  (opf_graph *sg, double *feat, int *label, int feat_n)

    bint       opf_graph_set_precomputed_distance (opf_graph *sg,
                                                  double *distance,
                                                  int *label)

    void       opf_graph_pdf_evaluate (opf_graph * sg)

cdef extern from "supervised.h":
    void opf_supervised_train    (opf_graph * sg)
    void opf_supervised_train_iterative (opf_graph *sg, double split)
    void opf_supervised_train_agglomerative (opf_graph *sg, double split)
    void opf_supervised_classify (opf_graph * sgtrain, double *feat, int sample_n, int *label)

cdef extern from "unsupervised.h":
    void opf_best_k_min_cut (opf_graph * sg, int kmin, int kmax)
    void opf_unsupervised_clustering (opf_graph * sg)
    void opf_unsupervised_knn_classify (opf_graph * sgtrain, double *feat, int sample_n, int *label)
