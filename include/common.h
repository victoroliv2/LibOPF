#ifndef _COMMON_H_
#define _COMMON_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <time.h>

#define TRUE  1
#define FALSE 0
#define NIL  -1

/* Error messages */

#define LOG_OUT_OF_MEMORY    "Cannot allocate memory space"
#define LOG_CANNOT_OPEN_FILE "Cannot open file"
#define LOG_INVALID_OPT      "Invalid option"

#define warning(msg) (fprintf(stderr, "Warning (%s:%d@%s): %s\n", __FILE__, __LINE__, __func__, #msg))
#define error(msg)   do { fprintf(stderr, "Error (%s:%d@%s): %s\n",   __FILE__, __LINE__, __func__, #msg); assert(0);} while(0)

#define ARCW_MAX 100000.0
#define DENS_MAX 1000.0

/* Common operations */

#ifndef MAX
#define MAX(x,y) (((x) > (y))?(x):(y))
#endif

#ifndef MIN
#define MIN(x,y) (((x) < (y))?(x):(y))
#endif

static inline void SWAP (int *x, int *y)
{
  const int v = *x;
  *x = *y;
  *y =  v;
}

/* Generates a random number within [low,high] */
int random_int (int low, int high);

float *alloc_float (size_t n);

int *alloc_int (size_t n);

#endif
