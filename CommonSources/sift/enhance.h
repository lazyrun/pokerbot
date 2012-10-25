/*
 *------------------------------------------------------------------------
 * Copyright (c) 2000, Vladimir N. Bichigov.
 *------------------------------------------------------------------------
 * enhance.h
 *------------------------------------------------------------------------
 * Library functions prototypes.
 *------------------------------------------------------------------------
 */

#ifndef __ENHANCE_H
#define __ENHANCE_H

#ifndef __VBCFG_H
#include "vbcfg.h"
#endif

#ifndef __VB_ERR_H
#include "vb_err.h"
#endif

#ifndef __VB_MEM_H
#include "vb_mem.h"
#endif

#ifndef __DFT_H
#include "dft.h"
#endif

#ifndef enhimpex_h
   #include "enhimpex.h"
#endif

#ifdef __cplusplus
extern "C" {

#endif
/*
 *------------------------------------------------------------------------
 * Histogram specification section
 *------------------------------------------------------------------------
 */

typedef enum {
  GLOBAL_OLD,
  SLIDING_OLD,
  INTERPOLATION_OLD
} MODE_TYPE;

typedef enum {
  LINEAR_OLD,
  CUPOLA_OLD,
  RAYLEIGH_OLD,
  EXPONENTIAL_OLD,
  POWER2DIV3_OLD,
  HYPERBOLIC_OLD,
  OTHER_OLD
} HIST_TYPE;

struct HIST_DEF {
  unsigned char **inp_img;
  unsigned char **out_img;
  int n_rows;
  int n_cols;
  int box;
  double *distrib;
  int *inverse;
  MODE_TYPE mode;
  HIST_TYPE type;
};

int  ENHANCE_IMPEX vb_form_pdist(int hist_type, int gmin, int gmax,
                                 double *pg, double a);
void ENHANCE_IMPEX make_global_inverse(unsigned char **row, int n_rows, int n_cols,
                                 double *h_distrib, int *inverse);
void ENHANCE_IMPEX vb_make_eql_inverse(unsigned char **row,int n_rows,int n_cols,
                                 int *inverse);
void ENHANCE_IMPEX vb_make_eql_inverse16(unsigned short **row,int n_rows,int n_cols,
                                 int *inverse);

int  ENHANCE_IMPEX vb_hist_mng(struct HIST_DEF*);
int  ENHANCE_IMPEX vb_form_distrib(struct HIST_DEF*,int,int,double);
int  ENHANCE_IMPEX vb_sliding_hist(struct HIST_DEF*);
int  ENHANCE_IMPEX vb_interpolation_hist(struct HIST_DEF*);
void ENHANCE_IMPEX vb_global_hist(struct HIST_DEF*);
int  ENHANCE_IMPEX vb_eql(unsigned char**,unsigned char**,int,int,int,int);
int  ENHANCE_IMPEX vb_eql16(unsigned short**,unsigned short**,int,int,int,int);
void ENHANCE_IMPEX vb_global_eql(unsigned char **row,unsigned char **out,
                                 int n_rows,int n_cols,int *ext_inverse);
void ENHANCE_IMPEX vb_global_eql16(unsigned short **row,unsigned short **out,
                                 int n_rows,int n_cols,int *ext_inverse);

/*
 *------------------------------------------------------------------------
 *------------------------------------------------------------------------
 */

struct CONV_DEF {
  unsigned char **inp_img;
  unsigned char **out_img;
  int n_rows;
  int n_cols;
  int box;
  PIX (*func)(unsigned char *, double level, double scale);
};

int ENHANCE_IMPEX vb_conv_mask(struct CONV_DEF*, double level, double scale);
PIX ENHANCE_IMPEX vb_laplace3x3_mask(PIX*, double level, double scale);
PIX ENHANCE_IMPEX vb_sobel3x3_mask(PIX* a, double level, double scale);
PIX ENHANCE_IMPEX vb_roberts3x3_mask(PIX* a, double level, double scale);

int ENHANCE_IMPEX vb_low_pass_flt(PIX**,PIX**,int,int,int);
int ENHANCE_IMPEX vb_high_pass_flt(PIX**,PIX**,int,int,int);
int ENHANCE_IMPEX vb_sharp(PIX**,PIX**,int,int,int,double);
int ENHANCE_IMPEX vbi_low_pass_flt(int**,int**,int,int,int);

int ENHANCE_IMPEX vb_stat_diff(unsigned char**,unsigned char**,int,int,int,double,double,double,double);

int ENHANCE_IMPEX vb_med_sort(PIX**,PIX**,int,int,int);
int ENHANCE_IMPEX vb_med_hist(unsigned char**,unsigned char**,int,int,int);

int ENHANCE_IMPEX vb_gauss3x3(PIX**,PIX**,int,int,double);
int ENHANCE_IMPEX vb_gauss5x5(PIX**,PIX**,int,int,double);
int ENHANCE_IMPEX vb_gauss7x7(PIX**,PIX**,int,int,double);

int ENHANCE_IMPEX vb_chg_res (double,unsigned char*[],int,int,unsigned char*[],int,int);
int ENHANCE_IMPEX vb_chg_resRGB (double,unsigned char*[],int,int,unsigned char*[],int,int);
int ENHANCE_IMPEX vb_zoom(double,unsigned char*[],int,int,int,unsigned char*[],int,int);

int ENHANCE_IMPEX vb_chg_res16 (double,unsigned short*[],int,int,unsigned short*[],int,int);
int ENHANCE_IMPEX vb_chg_resRGB16 (double,unsigned short*[],int,int,unsigned short*[],int,int);
int ENHANCE_IMPEX vb_zoom16(double,unsigned short*[],int,int,int,unsigned short*[],int,int);

int ENHANCE_IMPEX vb_conv2d(PIX**,PIX**,int,int,double*,int,int);
void ENHANCE_IMPEX vb_design_gauss_mask(double*,int,double);

int ENHANCE_IMPEX vb_root_flt(PIX**,PIX**,int,int,double);
int ENHANCE_IMPEX vb_cepstrum(PIX**,PIX**,int,int,double);


// Color versions:
void ENHANCE_IMPEX cmake_global_inverse(unsigned char **row,int n_rows,int n_cols,
                                        double *h_distrib,int *inverse,int WinRGB);
void ENHANCE_IMPEX cvb_make_eql_inverse(unsigned char **row,int n_rows,int n_cols,
                                        int *inverse,int WinRGB);

int  ENHANCE_IMPEX cvb_hist_mng( struct HIST_DEF *hist, int WinRGB);
void ENHANCE_IMPEX cvb_global_hist(struct HIST_DEF *hist, int WinRGB);
int  ENHANCE_IMPEX cvb_sliding_hist(struct HIST_DEF *gist, int WinRGB);
int  ENHANCE_IMPEX cvb_interpolation_hist(struct HIST_DEF *hist, int WinRGB);
int  ENHANCE_IMPEX cvb_eql(unsigned char **row,unsigned char **out,
                           int n_rows,int n_cols,int step,int boxsize,int WinRGB);
void ENHANCE_IMPEX cvb_global_eql(unsigned char **row,unsigned char **out,
                                 int n_rows,int n_cols,int *ext_inverse,int WinRGB);

int ENHANCE_IMPEX cvb_conv_mask(struct CONV_DEF *conv, double level, double scale,int WinRGB);

int ENHANCE_IMPEX cvb_low_pass_flt(PIX **row,PIX **out,int n_rows,int n_cols,int n);
int ENHANCE_IMPEX cvb_high_pass_flt(PIX **row,PIX **out,int n_rows,int n_cols,int n);
int ENHANCE_IMPEX cvb_sharp(PIX **row,PIX **out,int n_rows,int n_cols,int n,double coeff);
int ENHANCE_IMPEX cvbi_low_pass_flt(int **row,int **out,int n_rows,int n_cols,int n);

int ENHANCE_IMPEX cvb_stat_diff(unsigned char **row,unsigned char **out,int n_rows,int n_cols,
                  int n,double avr,double sigma,double coeff0,double coeff1);

int ENHANCE_IMPEX cvb_med_sort(PIX **row,PIX **out,int n_rows,int n_cols,int n,int WinRGB);
int ENHANCE_IMPEX cvb_med_hist(unsigned char **row,unsigned char **out,int n_rows,int n_cols,
                 int n,int WinRGB);

int ENHANCE_IMPEX cvb_gauss3x3(PIX *row[],PIX *flt[],int n_rows,int n_cols,double coeff);
int ENHANCE_IMPEX cvb_gauss5x5(PIX *row[],PIX *flt[],int n_rows,int n_cols,double coeff);
int ENHANCE_IMPEX cvb_gauss7x7(PIX *row[],PIX *flt[],int n_rows,int n_cols,double coeff);

int ENHANCE_IMPEX cvb_conv2d(PIX **img,PIX **out,int n_rows,int n_cols,double *mask,
               int m,int n,int WinRGB);

int ENHANCE_IMPEX cvb_root_flt(PIX **img,PIX **out,int n_rows,int n_cols,double root,int WinRGB);
int ENHANCE_IMPEX cvb_cepstrum(PIX **img,PIX **out,int n_rows,int n_cols,double scale,int WinRGB);

int ENHANCE_IMPEX Laplace3x3_Conv(PIX **inp, PIX **out, int n_rows, int n_cols,
											 double level, double scale);
int ENHANCE_IMPEX Sobel3x3_Conv(PIX **inp, PIX **out, int n_rows, int n_cols,
											 double level, double scale);
int ENHANCE_IMPEX Roberts3x3_Conv(PIX **inp, PIX **out, int n_rows, int n_cols,
											 double level, double scale);
int ENHANCE_IMPEX CLaplace3x3_Conv(PIX **inp, PIX **out, int n_rows, int n_cols,
											 double level, double scale,int WinRGB);
int ENHANCE_IMPEX CSobel3x3_Conv(PIX **inp, PIX **out, int n_rows, int n_cols,
											 double level, double scale,int WinRGB);
int ENHANCE_IMPEX CRoberts3x3_Conv(PIX **inp, PIX **out, int n_rows, int n_cols,
											 double level, double scale,int WinRGB);
//int ENHANCE_IMPEX GaussNxN_Conv(PIX **img,PIX **out,int n_rows,int n_cols,int n_mask,double coeff);
//int ENHANCE_IMPEX CGaussNxN_Conv(PIX **img,PIX **out,int n_rows,int n_cols,int n_mask,double coeff,int WinRGB);
int ENHANCE_IMPEX oldHistSpec(PIX **img,PIX **out,int n_rows,int n_cols,
                           MODE_TYPE mode,HIST_TYPE type, int box,

                           double gmin, double gmax, double param,

                           int *invers);

int ENHANCE_IMPEX oldCHistSpec(PIX **img,PIX **out,int n_rows,int n_cols,
                           MODE_TYPE mode,HIST_TYPE type, int box,

                           double gmin, double gmax, double param,

                           int *invers, int WinRGB);

void ENHANCE_IMPEX rgb2gray(PIX **row,PIX **out,int n_rows,int n_cols,int WinRGB);
void ENHANCE_IMPEX gray2rgb(PIX **row,PIX **out,int n_rows,int n_cols);

// не очень корректно рядом с PIX
void ENHANCE_IMPEX rgb2gray_16b(unsigned short **row,unsigned short **out,int n_rows,int n_cols,int WinRGB);
void ENHANCE_IMPEX gray2rgb_16b(unsigned short **row,unsigned short **out,int n_rows,int n_cols);

//int ENHANCE_IMPEX MedianFilter (unsigned char **row, unsigned char **out,
//                                int n_rows, int n_cols, int n);
//int ENHANCE_IMPEX CMedianFilter(unsigned char **row, unsigned char **out,
//                                int n_rows, int n_cols, int n, int WinRGB);

void ENHANCE_IMPEX LogTenPFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, double coeff);
void ENHANCE_IMPEX rgbLogTenPFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, double coeff, int WinRGB);
void ENHANCE_IMPEX LogNatPFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, double coeff);
void ENHANCE_IMPEX rgbLogNatPFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, double coeff, int WinRGB);
void ENHANCE_IMPEX SqrtPFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, double coeff);
void ENHANCE_IMPEX rgbSqrtPFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, double coeff, int WinRGB);
void ENHANCE_IMPEX SquaredPFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, double coeff);
void ENHANCE_IMPEX rgbSquaredPFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, double coeff, int WinRGB);

struct WhiteNoisePrms
{
   unsigned Seed;
   unsigned Ro, Ampl;
   int Channel;
   int W, x0, y0;
};

void ENHANCE_IMPEX AddWhiteNoisePFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, struct WhiteNoisePrms *Prms);
void ENHANCE_IMPEX rgbAddWhiteNoisePFun(unsigned char **Src, unsigned char **Dst,
                              int w, int h, struct WhiteNoisePrms *Prms, int WinRGB);

#ifdef __cplusplus
}
#endif

#endif /* __ENHANCE_H */














