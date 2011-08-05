/*
  Copyright (C) <2009-2011> <Alexandre Xavier Falcão and João Paulo Papa>

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
  Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  please see full copyright in COPYING file.

  -------------------------------------------------------------------------
  written by A.X. Falcão <afalcao@ic.unicamp.br> and by J.P. Papa
  <papa.joaopaulo@gmail.com>, Oct 20th 2008

  This program is a collection of functions to manage the Optimum-Path Forest (OPF)
  classifier.*/

#include "common.h"

/*
 * Generates a random number within [low,high].
 */
int
random_int (int low, int high)
{
  int k;
  double d;

  d = (double) rand () / ((double) RAND_MAX);
  k = d * (high - low);
  return low + k;
}
