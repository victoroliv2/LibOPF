#include <math.h>
#include "common.h"

#define __METRICS_MAIN__
#include "metrics.h"
#undef  __METRICS_MAIN__

/* euclidean */
  double d_eucl (double *f1, double *f2, int n)
{
  int     i;
  double   dist = 0.0f;

  for (i = 0; i < n; i++)
    dist += (f1[i] - f2[i]) * (f1[i] - f2[i]);

  return (dist);
}

/* log-euclidian */
  double d_eucl_log (double *f1, double *f2, int n)
{
  return (((double) ARCW_MAX * log (d_eucl (f1, f2, n) + 1)));
}

/* gaussian */
/*
  double d_gauss (double *f1, double *f2, int n, double gamma)
{
  int     i;
  double   dist = 0.0f;

  for (i = 0; i < n; i++)
    dist += (f1[i] - f2[i]) * (f1[i] - f2[i]);

  dist = exp (-gamma * sqrtf (dist));

  return (dist);
}
*/

/* chi-squared */
  double d_chi_square (double *f1, double *f2, int n)
{
  int     i;
  double   dist = 0.0f, sf1 = 0.0f, sf2 = 0.0f;

  for (i = 0; i < n; i++)
    {
      sf1 += f1[i];
      sf2 += f2[i];
    }

  for (i = 0; i < n; i++)
    dist +=
      1 / (f1[i] + f2[i] + 0.000000001) * pow (f1[i] / sf1 - f2[i] / sf2, 2);

  return (sqrtf (dist));
}

/* manhattan */
  double d_manhattan (double *f1, double *f2, int n)
{
  int     i;
  double   dist = 0.0f;

  for (i = 0; i < n; i++)
    dist += fabs (f1[i] - f2[i]);

  return (dist);
}

/* camberra */
  double d_canberra (double *f1, double *f2, int n)
{
  int     i;
  double   dist = 0.0f, aux;

  for (i = 0; i < n; i++)
    {
      aux = fabs (f1[i] + f2[i]);
      if (aux > 0)
        dist += (fabs (f1[i] - f2[i]) / aux);
    }

  return (dist);
}

/* squared chord */
  double d_squared_chord (double *f1, double *f2, int n)
{
  int     i;
  double   dist = 0.0f, aux1, aux2;

  for (i = 0; i < n; i++)
    {
      aux1 = sqrtf (f1[i]);
      aux2 = sqrtf (f2[i]);

      if ((aux1 >= 0) && (aux2 >= 0))
        dist += pow (aux1 - aux2, 2);
    }

  return (dist);
}

/* squared chi-squared */
  double d_squared_chi_square (double *f1, double *f2, int n)
{
  int     i;
  double   dist = 0.0f, aux;

  for (i = 0; i < n; i++)
    {
      aux = fabs (f1[i] + f2[i]);
      if (aux > 0)
        dist += (pow (f1[i] - f2[i], 2) / aux);
    }

  return (dist);
}

/* bray curtis */
  double d_bray_curtis (double *f1, double *f2, int n)
{
  int     i;
  double   dist = 0.0f, aux;

  for (i = 0; i < n; i++)
    {
      aux = f1[i] + f2[i];
      if (aux > 0)
        dist += (fabs (f1[i] - f2[i]) / aux);
    }

  return (dist);
}
