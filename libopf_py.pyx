# code

cimport libopf_py
import numpy as np
cimport numpy as np
from libc.stdlib cimport malloc, free

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
          metric="euclidian"):

    self.sg = libopf_py.subgraph_create (<int>Y.shape[0])
    if self.sg == NULL:
      raise MemoryError("Seems we've run out of of memory")

    if not libopf_py.subgraph_set_data (self.sg, <float*>X.data, <int*>Y.data, <int>X.shape[1]):
        raise MemoryError("Seems we've run out of of memory")

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

    libopf_py.subgraph_set_metric (self.sg, d[metric])
    libopf_py.supervised_train (self.sg)

  def predict(self, np.ndarray[np.float32_t, ndim=2, mode='c'] X):

    cdef np.ndarray[np.int32_t, ndim=1, mode='c'] labels
    labels = np.empty(X.shape[0], dtype=np.int32)

    libopf_py.supervised_classify (self.sg, <float*>X.data, <int>X.shape[0], <int*>labels.data)

    return labels
