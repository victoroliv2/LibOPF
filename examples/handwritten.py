import time

import numpy
import libopf_py

from scikits.learn import datasets, svm, metrics

digits = datasets.load_digits()

# To apply an classifier on this data, we need to flatten the image, to
# turn the data in a (samples, feature) matrix:
n_samples = len(digits.images)
data = digits.images.reshape((n_samples, -1))

def run(split):
  n_split = int(split*n_samples)

  print ""
  print "="*100
  print ""

  print "Split: %3.2f" % split
  print "Size: %d, Classifying Size: %d, Testing Size: %d" % (n_samples, n_split, n_samples-n_split)

  rand = numpy.random.permutation(n_samples)

  random_data  = data[rand]
  random_label = digits.target[rand]

  data_train,  data_test  = random_data [:n_split], random_data [n_split:]
  label_train, label_test = random_label[:n_split], random_label[n_split:]

  print "-"*20, "OPF", "-"*20
  def opf():

    # OPF only supports 32 bits datatypes at the moment
    data_train_32  = data_train.astype(numpy.float32)
    label_train_32 = label_train.astype(numpy.int32)
    data_test_32   = data_test.astype(numpy.float32)
    label_test_32  = label_test.astype(numpy.int32)

    O = libopf_py.OPF()

    t = time.time()
    O.fit(data_train_32, label_train_32, learning="agglomerative", split=0.8)
    print "OPF: time elapsed in fitting: %f secs" % (time.time()-t)

    t = time.time()
    predicted = O.predict(data_test_32)
    print "OPF: time elapsed in predicting: %f secs" % (time.time()-t)

    print "Classification report for OPF:\n%s\n" % (metrics.classification_report(label_test_32, predicted))
    print "Confusion matrix:\n%s" % metrics.confusion_matrix(label_test_32, predicted)

  opf()

  print "-"*20, "SVM", "-"*20
  def _svm():

    clf = svm.SVC()

    t = time.time()
    clf.fit(data_train, label_train)
    print "SVM: time elapsed in fitting: %f secs" % (time.time()-t)

    t = time.time()
    predicted = clf.predict(data_test)
    print "SVM: time elapsed in predicting: %f secs" % (time.time()-t)

    print "Classification report for SVM:\n%s\n" % (metrics.classification_report(label_test, predicted))
    print "Confusion matrix:\n%s" % metrics.confusion_matrix(label_test, predicted)

  _svm()

run(0.1)
run(0.2)
run(0.4)
run(0.6)
run(0.8)
