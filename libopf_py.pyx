# code

cimport libopf_py
import numpy as np
cimport numpy as np

cdef class OPF:
  cdef libopf_py.subgraph * sg
  def __cinit__(self):
      self.sg = NULL

  def __dealloc__(self):
    if self.sg is not NULL:
      libopf_py.subgraph_destroy (&self.sg)

  def fit(self,
          np.ndarray[np.float32_t, ndim=2, mode='c'] X,
          np.ndarray[np.int32_t,   ndim=1, mode='c'] Y,
          learning="default", metric="euclidian",
          bint use_precomputed_distance=True, double split=0.2):

    cdef np.ndarray[np.float32_t, ndim=2, mode='c'] X_train, X_eval
    cdef np.ndarray[np.float32_t, ndim=1, mode='c'] Y_train, Y_eval
    cdef int train_size, eval_size

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

    if learning not in ("default", "iterative", "agglomerative"):
      raise Exception("Invalid training mode")

    #split training set
    if learning in ("iterative", "agglomerative"):
      train_size = int(Y.shape[0] * split)
      eval_size = Y.shape[0] - train_size
    else:
      train_size = Y.shape[0]
      eval_size = 0

    self.sg = libopf_py.subgraph_create (<int>train_size)
    if self.sg == NULL:
      raise MemoryError("Seems we've run out of of memory")

    if learning in ("iterative", "agglomerative"):
      X_train, X_eval = X[:train_size], X[train_size:]
      Y_train, Y_eval = Y[:train_size], Y[train_size:]
      if not libopf_py.subgraph_set_data (self.sg, <float*>X_train.data,
                                          <int*>Y_train.data, <int>X_train.shape[1]):
        raise MemoryError("Seems we've run out of of memory")
    else:
      if not libopf_py.subgraph_set_data (self.sg, <float*>X.data, <int*>Y.data, <int>X.shape[1]):
        raise MemoryError("Seems we've run out of of memory")

    if use_precomputed_distance:
      libopf_py.subgraph_precompute_distance (self.sg, NULL, d[metric])
    else:
      libopf_py.subgraph_set_metric (self.sg, d[metric])

    if learning == "default":
      libopf_py.supervised_train (self.sg)
    elif learning == "iterative":
      libopf_py.supervised_train_iterative (self.sg, <float*>X_eval.data,
                                            <int*>Y_eval.data, <int>eval_size)
    elif learning == "agglomerative":
      libopf_py.supervised_train_agglomerative (self.sg, <float*>X_eval.data,
                                                <int*>Y_eval.data, <int>eval_size)

  def predict(self, np.ndarray[np.float32_t, ndim=2, mode='c'] X):

    cdef np.ndarray[np.int32_t, ndim=1, mode='c'] labels
    labels = np.empty(X.shape[0], dtype=np.int32)

    libopf_py.supervised_classify (self.sg, <float*>X.data, <int>X.shape[0], <int*>labels.data)

    return labels
