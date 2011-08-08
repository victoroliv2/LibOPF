#define __METRICS_MAIN__
#include "metrics.h"
#undef __METRICS_MAIN__

/* euclidean */
static  float d_eucl (float *f1, float *f2, int n)
{
  int     i;
  float   dist = 0.0f;

  for (i = 0; i < n; i++)
    dist += (f1[i] - f2[i]) * (f1[i] - f2[i]);

  return (dist);
}

/* log-euclidian */
static  float d_eucl_log (float *f1, float *f2, int n)
{
  return (((float) ARCW_MAX * log (d_eucl (f1, f2, n) + 1)));
}

/* gaussian */
static  float d_gauss (float *f1, float *f2, int n, float gamma)
{
  int     i;
  float   dist = 0.0f;

  for (i = 0; i < n; i++)
    dist += (f1[i] - f2[i]) * (f1[i] - f2[i]);

  dist = exp (-gamma * sqrtf (dist));

  return (dist);
}

/* chi-squared */
static  float d_chi_square (float *f1, float *f2, int n)
{
  int     i;
  float   dist = 0.0f, sf1 = 0.0f, sf2 = 0.0f;

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
static  float d_manhattan (float *f1, float *f2, int n)
{
  int     i;
  float   dist = 0.0f;

  for (i = 0; i < n; i++)
    dist += fabs (f1[i] - f2[i]);

  return (dist);
}

/* camberra */
static  float d_canberra (float *f1, float *f2, int n)
{
  int     i;
  float   dist = 0.0f, aux;

  for (i = 0; i < n; i++)
    {
      aux = fabs (f1[i] + f2[i]);
      if (aux > 0)
        dist += (fabs (f1[i] - f2[i]) / aux);
    }

  return (dist);
}

/* squared chord */
static  float d_squared_chord (float *f1, float *f2, int n)
{
  int     i;
  float   dist = 0.0f, aux1, aux2;

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
static  float d_squared_chi_square (float *f1, float *f2, int n)
{
  int     i;
  float   dist = 0.0f, aux;

  for (i = 0; i < n; i++)
    {
      aux = fabs (f1[i] + f2[i]);
      if (aux > 0)
        dist += (pow (f1[i] - f2[i], 2) / aux);
    }

  return (dist);
}

/* bray curtis */
static  float d_bray_curtis (float *f1, float *f2, int n)
{
  int     i;
  float   dist = 0.0f, aux;

  for (i = 0; i < n; i++)
    {
      aux = f1[i] + f2[i];
      if (aux > 0)
        dist += (fabs (f1[i] - f2[i]) / aux);
    }

  return (dist);
}

void
set_metric (METRIC m)
{
  switch (m)
  {
    case EUCLIDIAN:
      arc_weight = d_eucl;
      break;
    case LOG_EUCLIDIAN:
      arc_weight = d_eucl_log;
      break;
    case GAUSSIAN:
      arc_weight = d_gauss;
      break;
    case CHI_SQUARE:
      arc_weight = d_chi_square;
      break;
    case MANHATTAN:
      arc_weight = d_manhattan;
      break;
    case CANBERRA:
      arc_weight = d_canberra;
      break;
    case SQUARED_CHORD:
      arc_weight = d_squared_chord;
      break;
    case SQUARED_CHI_SQUARE:
      arc_weight = d_squared_chi_square;
      break;
    case BRAY_CURTIS:
      arc_weight = d_bray_curtis;
      break;
    default:
      error ("Undefinied metric");
  }
}
