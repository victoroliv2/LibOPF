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

int *AllocIntArray(int n)
{
  int *v=NULL;
  v = (int *) calloc(n,sizeof(int));
  if (v == NULL)
    Error(MSG1,"AllocIntArray");
  return(v);
}

float *AllocFloatArray(int n)
{
  float *v=NULL;
  v = (float *) calloc(n,sizeof(float));
  if (v == NULL)
    Error(MSG1,"AllocFloatArray");
  return(v);
}

void Error(char *msg,char *func)
{
  /* It prints error message and exits
                                      the program. */
  fprintf(stderr,"Error:%s in %s\n",msg,func);
  exit(-1);
}

void Warning(char *msg,char *func)
{
  /* It prints warning message and
                                         leaves the routine. */
  fprintf(stdout,"Warning:%s in %s\n",msg,func);

}


inline void Change(int *a, int *b)  /* It changes content between a and b */
{
  const int c = *a;
  *a = *b;
  *b = c;
}

/*
 * Generates a random number within [low,high].
http://www.ime.usp.br/~pf/algoritmos/aulas/random.html
 */
int RandomInteger (int low, int high)
{
  int k;
  double d;

  d = (double) rand () / ((double) RAND_MAX);
  k = d * (high - low);
  return low + k;
}
