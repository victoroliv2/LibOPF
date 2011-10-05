# code

cimport libopf_py
import numpy as np
cimport numpy as np

cdef class OPF:

  cdef libopf_py.opf_graph * sg
  cdef bint supervised
  cdef bint precomputed_distance

  cdef int node_n
  cdef int feat_n

  def __cinit__(self):
      self.sg = NULL
      self.supervised = True
      self.precomputed_distance = False

  def __dealloc__(self):
    if self.sg is not NULL:
      libopf_py.opf_graph_destroy (&self.sg)

  def fit (self,
          np.ndarray[np.float64_t, ndim=2, mode='c'] X,
          np.ndarray[np.int32_t,   ndim=1, mode='c'] Y = None,
          learning="default", metric="euclidian",
          bint precomputed_distance=False, double split=0.8):

      d = {
            "euclidian"          : libopf_py.EUCLIDIAN,
            "log_euclidian"      : libopf_py.LOG_EUCLIDIAN,
            "chi_square"         : libopf_py.CHI_SQUARE,
            "manhattan"          : libopf_py.MANHATTAN,
            "canberra"           : libopf_py.CANBERRA,
            "squared_chord"      : libopf_py.SQUARED_CHORD,
            "squared_chi_square" : libopf_py.SQUARED_CHI_SQUARE,
            "bray_curtis"        : libopf_py.BRAY_CURTIS
          }

      if Y != None:
        self.supervised = True
      else:
        self.supervised = False

      self.precomputed_distance = precomputed_distance

      self.node_n = <int>X.shape[0]
      self.feat_n = <int>X.shape[1]

      if Y and X.shape[0] != Y.shape[0]:
        raise Exception("Shape mismatch")

      if self.precomputed_distance and X.shape[0] != X.shape[1]:
        raise Exception("Distance matrix should be squared, but it's (%s,%s)" %
                                                     (X.shape[0], X.shape[1]))

      if self.supervised:
        if learning not in ("default", "iterative", "agglomerative"):
          raise Exception("Invalid training mode")

      self.sg = libopf_py.opf_graph_create (<int>X.shape[0])
      if self.sg == NULL:
        raise MemoryError("Seems we've run out of of memory")

      if self.precomputed_distance:
        if self.supervised:
          if not libopf_py.opf_graph_set_precomputed_distance (self.sg,
                                                              <double*>X.data,
                                                              <int*>Y.data):
            raise MemoryError("Seems we've run out of of memory")
        else:
          if not libopf_py.opf_graph_set_precomputed_distance (self.sg,
                                                              <double*>X.data,
                                                              NULL):
            raise MemoryError("Seems we've run out of of memory")

      else:

        if self.supervised:
          if not libopf_py.opf_graph_set_feature (self.sg,
                                                 <double*>X.data,
                                                 <int*>Y.data,
                                                 <int>X.shape[1]):
            raise MemoryError("Seems we've run out of of memory")
        else:
          if not libopf_py.opf_graph_set_feature (self.sg,
                                                 <double*>X.data,
                                                 NULL,
                                                 <int>X.shape[1]):
            raise MemoryError("Seems we've run out of of memory")

        libopf_py.opf_graph_set_metric (self.sg, NULL, d[metric])

      if self.supervised:
        if learning == "default":
          libopf_py.opf_supervised_train (self.sg)
        elif learning == "iterative":
          libopf_py.opf_supervised_train_iterative (self.sg, split)
        elif learning == "agglomerative":
          libopf_py.opf_supervised_train_agglomerative (self.sg, split)
      else:
        libopf_py.opf_best_k_min_cut (self.sg, 1, 10)
        libopf_py.opf_unsupervised_clustering (self.sg)

  def predict(self, np.ndarray[np.float64_t, ndim=2, mode='c'] X):

    cdef np.ndarray[np.int32_t, ndim=1, mode='c'] labels

    if self.precomputed_distance:
      labels = np.empty(X.shape[1], dtype=np.int32)
    else:
      labels = np.empty(X.shape[0], dtype=np.int32)

    if self.supervised == None:
      raise Exception ("Not fitted!")

    if self.precomputed_distance and X.shape[0] != self.node_n:
      raise Exception("Distance matrix shape is wrong")

    if not self.precomputed_distance and X.shape[1] != self.feat_n:
      raise Exception("Feature matrix shape is wrong")

    if self.precomputed_distance:
      if self.supervised:
        libopf_py.opf_supervised_classify (self.sg,
                                           <double*>X.data,
                                           <int>X.shape[1],
                                           <int*>labels.data)
      else:
        libopf_py.opf_unsupervised_knn_classify (self.sg,
                                                 <double*>X.data,
                                                 <int>X.shape[1],
                                                 <int*>labels.data)
    else:
      if self.supervised:
        libopf_py.opf_supervised_classify (self.sg,
                                           <double*>X.data,
                                           <int>X.shape[0],
                                           <int*>labels.data)
      else:
        libopf_py.opf_unsupervised_knn_classify (self.sg,
                                                 <double*>X.data,
                                                 <int>X.shape[0],
                                                 <int*>labels.data)

    return labels
