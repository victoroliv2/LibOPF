#ifndef _METRICS_H_
#define _METRICS_H_

#ifdef __METRICS_MAIN__
double d_eucl                (double *f1, double *f2, int n);
double d_eucl_log            (double *f1, double *f2, int n);
double d_chi_square          (double *f1, double *f2, int n);
double d_manhattan           (double *f1, double *f2, int n);
double d_canberra            (double *f1, double *f2, int n);
double d_squared_chord       (double *f1, double *f2, int n);
double d_squared_chi_square  (double *f1, double *f2, int n);
double d_bray_curtis         (double *f1, double *f2, int n);
#else
extern double d_eucl                (double *f1, double *f2, int n);
extern double d_eucl_log            (double *f1, double *f2, int n);
extern double d_chi_square          (double *f1, double *f2, int n);
extern double d_manhattan           (double *f1, double *f2, int n);
extern double d_canberra            (double *f1, double *f2, int n);
extern double d_squared_chord       (double *f1, double *f2, int n);
extern double d_squared_chi_square  (double *f1, double *f2, int n);
extern double d_bray_curtis         (double *f1, double *f2, int n);
#endif

#endif
