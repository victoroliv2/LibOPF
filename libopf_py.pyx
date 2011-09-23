# code

cimport libopf_py
import numpy as np
cimport numpy as np

cdef class OPF:

  cdef libopf_py.subgraph * sg
  cdef bint supervised

  cdef int node_n
  cdef int feat_n

  def __cinit__(self):
      self.sg = NULL
      self.supervised = True

  def __dealloc__(self):
    if self.sg is not NULL:
      libopf_py.subgraph_destroy (&self.sg)

  def fit (self,
          np.ndarray[np.float32_t, ndim=2, mode='c'] X,
          np.ndarray[np.int32_t,   ndim=1, mode='c'] Y = None,
          learning="default", metric="euclidian",
          bint use_precomputed_distance=False, float split=0.8):

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

      if X.shape[0] != Y.shape[0]:
        raise Exception("Shape mismatch")

      if use_precomputed_distance and X.shape[0] != X.shape[1]:
        raise Exception("Distance matrix should be squared, but is (%s,%s)" % (X.shape[0], X.shape[1]))

      if Y != None:
        self.supervised = True
      else:
        self.supervised = False

      self.node_n = <int>X.shape[0]
      self.feat_n = <int>X.shape[1]

      if self.supervised:
        if learning not in ("default", "iterative", "agglomerative"):
          raise Exception("Invalid training mode")

      self.sg = libopf_py.subgraph_create (<int>X.shape[0])
      if self.sg == NULL:
        raise MemoryError("Seems we've run out of of memory")

      if self.supervised:
        if not libopf_py.subgraph_set_feature (self.sg, <float*>X.data, <int*>Y.data, <int>X.shape[1]):
          raise MemoryError("Seems we've run out of of memory")
      else:
        if not libopf_py.subgraph_set_feature (self.sg, <float*>X.data, NULL, <int>X.shape[1]):
          raise MemoryError("Seems we've run out of of memory")

      libopf_py.subgraph_set_metric (self.sg, NULL, d[metric])

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

  def predict(self, np.ndarray[np.float32_t, ndim=2, mode='c'] X):

    cdef np.ndarray[np.int32_t, ndim=1, mode='c'] labels
    labels = np.empty(X.shape[0], dtype=np.int32)

    if self.supervised == None:
      raise Exception ("Not fitted!")

    if X.shape[1] != self.feat_n:
      raise Exception("Feature matrix shape is wrong")

    if self.supervised:
      libopf_py.opf_supervised_classify (self.sg, <float*>X.data, <int>X.shape[0], <int*>labels.data)
    else:
      libopf_py.opf_unsupervised_knn_classify (self.sg, <float*>X.data, <int>X.shape[0], <int*>labels.data)

    return labels
