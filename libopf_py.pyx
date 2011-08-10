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
          int metric=0):

    self.sg = libopf_py.subgraph_create (<int>Y.shape[0])
    if self.sg == NULL:
      raise MemoryError("Seems we've run out of of memory")

    if not libopf_py.subgraph_set_data (self.sg, <float*>X.data, <int*>Y.data, <int>X.shape[0], <int>X.shape[1]):
        raise MemoryError("Seems we've run out of of memory")

    libopf_py.subgraph_set_metric (self.sg, <METRIC>metric)

  def predict(self, np.ndarray[np.float32_t, ndim=2, mode='c'] X):

    cdef np.ndarray[np.int32_t, ndim=1, mode='c'] labels
    labels = np.empty(X.shape[0], dtype=np.int32)

    libopf_py.supervised_classify (self.sg, <float*>X.data, <int>X.shape[0], <int*>labels.data)

    return labels
