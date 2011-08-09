#ifndef _METRICS_H_
#define _METRICS_H_

#ifdef __METRICS_MAIN__
float d_eucl                (float *f1, float *f2, int n);
float d_eucl_log            (float *f1, float *f2, int n);
float d_chi_square          (float *f1, float *f2, int n);
float d_manhattan           (float *f1, float *f2, int n);
float d_canberra            (float *f1, float *f2, int n);
float d_squared_chord       (float *f1, float *f2, int n);
float d_squared_chi_square  (float *f1, float *f2, int n);
float d_bray_curtis         (float *f1, float *f2, int n);
#else
extern float d_eucl                (float *f1, float *f2, int n);
extern float d_eucl_log            (float *f1, float *f2, int n);
extern float d_chi_square          (float *f1, float *f2, int n);
extern float d_manhattan           (float *f1, float *f2, int n);
extern float d_canberra            (float *f1, float *f2, int n);
extern float d_squared_chord       (float *f1, float *f2, int n);
extern float d_squared_chi_square  (float *f1, float *f2, int n);
extern float d_bray_curtis         (float *f1, float *f2, int n);
#endif

#endif
