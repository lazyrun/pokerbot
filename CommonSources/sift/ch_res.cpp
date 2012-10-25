/*
 *------------------------------------------------------------------------------
 * Copyright (C) 2000, Vladimir N. Bichigov.
 *------------------------------------------------------------------------------
 *
 *  int vb_chg_res
 *  int vb_chg_resRGB
 *  int vb_zoom
 *
 *---------------------------------------------------------------------------
 *  Procedures to change image scale using bilinear interpolation.
 *  ChgRes, ChgResRGB :
 *  Parameter "ratio" defines ratio of target image to source image scale
 *  Optimal range of "ratio" parameter 0.5 - 2.0.
 *  Zoom:
 *  Useful when ratio smaller than 0.5. This procedure diminish image
 *  in two steps: at first slightly increase and then fold N times.
 *  bpp parameter has value 1 for gray images and 3 for RGB images
 *---------------------------------------------------------------------------
 */
#include    <math.h>

#include    "enhance.h"

/*----------------------------------------------------------------------*/
int vb_chg_res
/*----------------------------------------------------------------------*/
(
double          ratio,
unsigned char   *ini_row[],
int             n_ini_rows,
int             n_ini_cols,
unsigned char   *row[],
int             n_rows,
int             n_cols
)
/*----------------------------------------------------------------------*/
{
    unsigned char   *img_ptr;

    int             *mult_x0;
    int             *mult_x1;
    int             *mult_y0;
    int             *mult_y1;

    int             *x0;
    int             *x1;
    int             *y0;
    int             *y1;

    int             i,j;
    int             i0,i1;

    int             cx0,cx1;
    int             cy0,cy1;

    int             cmx0,cmx1;
    int             cmy0,cmy1;

    int             *x0_ptr;
    int             *x1_ptr;

    int             *mx0_ptr;
    int             *mx1_ptr;

    double          c0,cy,cx;


    /*----------------------------------------------------------*/
    /* If resolution not changed, then just copy ini_img -> img */
    /*----------------------------------------------------------*/
    if (ratio == 1.0)
    {
        for (i=0; i < n_rows; ++i)
        {
            img_ptr = row[i];

            if (i < n_ini_rows) i0 = i;
            else                i0 = n_ini_rows-1;

            for (j=0; j < n_cols; ++j)
            {
                if (j < n_ini_cols) i1 = j;
                else                i1 = n_ini_cols-1;

                *img_ptr++ = *(ini_row[i0]+i1);
            }
        }
        return (VB_OK);
    }
    /*----------------------------------------------------------*/

    y0 = (int*) vb_malloc (sizeof(int)*n_rows);
    if (y0 == NULL) return (VB_MEM);
    y1 = (int*) vb_malloc (sizeof(int)*n_rows);
    if (y1 == NULL)
    {
      vb_free (y0);
      return (VB_MEM);
    }

    mult_y0 = (int*) vb_malloc (sizeof(int)*n_rows);
    if (mult_y0 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      return (VB_MEM);
    }
    mult_y1 = (int*) vb_malloc (sizeof(int)*n_rows);
    if (mult_y1 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      return (VB_MEM);
    }

    x0 = (int*) vb_malloc (sizeof(int)*n_cols);
    if (x0 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      vb_free (mult_y1);
      return (VB_MEM);
    }
    x1 = (int*) vb_malloc (sizeof(int)*n_cols);
    if (x1 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      vb_free (mult_y1);
      vb_free (x0);
      return (VB_MEM);
    }

    mult_x0 = (int*) vb_malloc (sizeof(int)*n_cols);
    if (mult_x0 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      vb_free (mult_y1);
      vb_free (x0);
      vb_free (x1);
      return (VB_MEM);
    }
    mult_x1 = (int*) vb_malloc (sizeof(int)*n_cols);
    if (mult_x1 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      vb_free (mult_y1);
      vb_free (x0);
      vb_free (x1);
      vb_free (mult_x0);
      return (VB_MEM);
    }

    ratio = 1.0/ratio;
    c0 = ratio/2.0 - 0.5;

    /*----------------------------------------------------------*/
    /*  Definition interpolation points along Y                 */
    /*----------------------------------------------------------*/
    for (i=0; i < n_rows; ++i)
    {
        cy = c0 + i*ratio;

        if (cy < 0.0) cy = 0.0;

        cy0 = (int)cy;

        if (cy0 > (n_ini_rows-2))
        {
            cy0 = n_ini_rows-2;
            cy1 = cy0 + 1;

            cmy1 = 1024;
            cmy0 = 0;

            *(y0+i) = cy0;
            *(y1+i) = cy1;
            *(mult_y0+i) = cmy0;
            *(mult_y1+i) = cmy1;
        }
        else
        {
            cy1 = cy0 + 1;

            cmy1 = int(1024*(cy-cy0) + 0.5);
            cmy0 = 1024 - cmy1;

            *(y0+i) = cy0;
            *(y1+i) = cy1;
            *(mult_y0+i) = cmy0;
            *(mult_y1+i) = cmy1;
        }
    }


    /*----------------------------------------------------------*/
    /*  Definition interpolation points along X                 */
    /*----------------------------------------------------------*/
    for (i=0; i < n_cols; ++i)
    {
        cx = c0 + i*ratio;

        if (cx < 0.0) cx = 0.0;

        cx0 = (int)cx;

        if (cx0 > (n_ini_cols-2))
        {
            cx0 = n_ini_cols-2;
            cx1 = cx0 + 1;

            cmx1 = 1024;
            cmx0 = 0;

            *(x0+i) = cx0;
            *(x1+i) = cx1;
            *(mult_x0+i) = cmx0;
            *(mult_x1+i) = cmx1;
        }
        else
        {
            cx1 = cx0 + 1;

            cmx1 = int(1024*(cx-cx0) + 0.5);
            cmx0 = 1024 - cmx1;

            *(x0+i) = cx0;
            *(x1+i) = cx1;
            *(mult_x0+i) = cmx0;
            *(mult_x1+i) = cmx1;
        }
    }


    /*----------------------------------------------------------*/
    /*  Interpolation along X and Y                             */
    /*----------------------------------------------------------*/
    for (i=0; i < n_rows; ++i)
    {
        img_ptr = row[i];

        cy0 = *(y0+i);
        cy1 = *(y1+i);

        cmy0 = *(mult_y0+i);
        cmy1 = *(mult_y1+i);

        x0_ptr = x0;
        x1_ptr = x1;

        mx0_ptr = mult_x0;
        mx1_ptr = mult_x1;

        for (j=0; j < n_cols; ++j)
        {
            cx0 = *x0_ptr++;
            cx1 = *x1_ptr++;

            cmx0 = *mx0_ptr++;
            cmx1 = *mx1_ptr++;

            i0 = *(ini_row[cy0]+cx0) * cmy0 +
                 *(ini_row[cy1]+cx0) * cmy1;
            i0 = i0 >> 10;

            i1 = *(ini_row[cy0]+cx1) * cmy0 +
                 *(ini_row[cy1]+cx1) * cmy1;
            i1 = i1 >> 10;

            *img_ptr++  = (i0*cmx0 + i1*cmx1) >> 10;

        }
    }
    /*----------------------------------------------------------*/

    vb_free (x0);
    vb_free (x1);
    vb_free (y0);
    vb_free (y1);

    vb_free (mult_x0);
    vb_free (mult_x1);
    vb_free (mult_y0);
    vb_free (mult_y1);

    return (VB_OK);
}
/*----------------------------------------------------------------------*/
int vb_chg_resRGB
/*----------------------------------------------------------------------*/
(
double		ratio,
unsigned char   *ini_row[],
int             n_ini_rows,
int             n_ini_cols,
unsigned char   *row[],
int             n_rows,
int             n_cols
)
/*----------------------------------------------------------------------*/
{
    unsigned char   *img_ptr;

    int             *mult_x0;
    int             *mult_x1;
    int             *mult_y0;
    int             *mult_y1;

    int             *x0;
    int             *x1;
    int             *y0;
    int             *y1;

    int             i,j;
    int             i0,i1;

    int             cx0,cx1;
    int             cy0,cy1;

    int             cmx0,cmx1;
    int             cmy0,cmy1;

    int             *x0_ptr;
    int             *x1_ptr;

    int             *mx0_ptr;
    int             *mx1_ptr;

    double          c0,cy,cx;


    /*----------------------------------------------------------*/
    /* If resolution not changed, then just copy ini_img -> img */
    /*----------------------------------------------------------*/
    if (ratio == 1.0)
    {
        for (i=0; i < n_rows; ++i)
        {
            img_ptr = row[i];

            if (i < n_ini_rows) i0 = i;
            else                i0 = n_ini_rows-1;

            for (j=0; j < n_cols; ++j)
            {
                if (j < n_ini_cols) i1 = j;
                else                i1 = n_ini_cols-1;

                *img_ptr++ = *(ini_row[i0]+3*i1);
                *img_ptr++ = *(ini_row[i0]+3*i1+1);
                *img_ptr++ = *(ini_row[i0]+3*i1+2);
            }
        }
        return (VB_OK);
    }
    /*----------------------------------------------------------*/

    y0 = (int*) vb_malloc (sizeof(int)*n_rows);
    if (y0 == NULL) return (VB_MEM);
    y1 = (int*) vb_malloc (sizeof(int)*n_rows);
    if (y1 == NULL)
    {
      vb_free (y0);
      return (VB_MEM);
    }

    mult_y0 = (int*) vb_malloc (sizeof(int)*n_rows);
    if (mult_y0 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      return (VB_MEM);
    }
    mult_y1 = (int*) vb_malloc (sizeof(int)*n_rows);
    if (mult_y1 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      return (VB_MEM);
    }

    x0 = (int*) vb_malloc (sizeof(int)*n_cols);
    if (x0 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      vb_free (mult_y1);
      return (VB_MEM);
    }
    x1 = (int*) vb_malloc (sizeof(int)*n_cols);
    if (x1 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      vb_free (mult_y1);
      vb_free (x0);
      return (VB_MEM);
    }

    mult_x0 = (int*) vb_malloc (sizeof(int)*n_cols);
    if (mult_x0 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      vb_free (mult_y1);
      vb_free (x0);
      vb_free (x1);
      return (VB_MEM);
    }
    mult_x1 = (int*) vb_malloc (sizeof(int)*n_cols);
    if (mult_x1 == NULL)
    {
      vb_free (y0);
      vb_free (y1);
      vb_free (mult_y0);
      vb_free (mult_y1);
      vb_free (x0);
      vb_free (x1);
      vb_free (mult_x0);
      return (VB_MEM);
    }

    ratio = 1.0/ratio;
    c0 = ratio/2.0 - 0.5;

    /*----------------------------------------------------------*/
    /*  Definition interpolation points along Y                 */
    /*----------------------------------------------------------*/
    for (i=0; i < n_rows; ++i)
    {
        cy = c0 + i*ratio;

        if (cy < 0.0) cy = 0.0;

        cy0 = (int) cy;

        if (cy0 > (n_ini_rows-2))
        {
            cy0 = n_ini_rows-2;
            cy1 = cy0 + 1;

            cmy1 = 1024;
            cmy0 = 0;

            *(y0+i) = cy0;
            *(y1+i) = cy1;
            *(mult_y0+i) = cmy0;
            *(mult_y1+i) = cmy1;
        }
        else
        {
            cy1 = cy0 + 1;

            cmy1 = int(1024*(cy-cy0) + 0.5);
            cmy0 = 1024 - cmy1;

            *(y0+i) = cy0;
            *(y1+i) = cy1;
            *(mult_y0+i) = cmy0;
            *(mult_y1+i) = cmy1;
        }
    }


    /*----------------------------------------------------------*/
    /*  Definition interpolation points along X                 */
    /*----------------------------------------------------------*/
    for (i=0; i < n_cols; ++i)
    {
        cx = c0 + i*ratio;

        if (cx < 0.0) cx = 0.0;

        cx0 = (int) cx;

        if (cx0 > (n_ini_cols-2))
        {
            cx0 = n_ini_cols-2;
            cx1 = cx0 + 1;

            cmx1 = 1024;
            cmx0 = 0;

            *(x0+i) = cx0;
            *(x1+i) = cx1;
            *(mult_x0+i) = cmx0;
            *(mult_x1+i) = cmx1;
        }
        else
        {
            cx1 = cx0 + 1;

            cmx1 = int(1024*(cx-cx0) + 0.5);
            cmx0 = 1024 - cmx1;

            *(x0+i) = cx0;
            *(x1+i) = cx1;
            *(mult_x0+i) = cmx0;
            *(mult_x1+i) = cmx1;
        }
    }


    /*----------------------------------------------------------*/
    /*  Interpolation along X and Y                             */
    /*----------------------------------------------------------*/
    for (i=0; i < n_rows; ++i)
    {
        img_ptr = row[i];

        cy0 = *(y0+i);
        cy1 = *(y1+i);

        cmy0 = *(mult_y0+i);
        cmy1 = *(mult_y1+i);

        x0_ptr = x0;
        x1_ptr = x1;

        mx0_ptr = mult_x0;
        mx1_ptr = mult_x1;

        for (j=0; j < n_cols; ++j)
        {
            cx0 = *x0_ptr++;
            cx1 = *x1_ptr++;
	    cx0 *= 3;
	    cx1 *= 3;

            cmx0 = *mx0_ptr++;
            cmx1 = *mx1_ptr++;

            i0 = *(ini_row[cy0]+cx0) * cmy0 +
                 *(ini_row[cy1]+cx0) * cmy1;
            i0 = i0 >> 10;

            i1 = *(ini_row[cy0]+cx1) * cmy0 +
                 *(ini_row[cy1]+cx1) * cmy1;
            i1 = i1 >> 10;

            *img_ptr++  = (i0*cmx0 + i1*cmx1) >> 10;

            i0 = *(ini_row[cy0]+cx0+1) * cmy0 +
                 *(ini_row[cy1]+cx0+1) * cmy1;
            i0 = i0 >> 10;

            i1 = *(ini_row[cy0]+cx1+1) * cmy0 +
                 *(ini_row[cy1]+cx1+1) * cmy1;
            i1 = i1 >> 10;

            *img_ptr++  = (i0*cmx0 + i1*cmx1) >> 10;

            i0 = *(ini_row[cy0]+cx0+2) * cmy0 +
                 *(ini_row[cy1]+cx0+2) * cmy1;
            i0 = i0 >> 10;

            i1 = *(ini_row[cy0]+cx1+2) * cmy0 +
                 *(ini_row[cy1]+cx1+2) * cmy1;
            i1 = i1 >> 10;

            *img_ptr++  = (i0*cmx0 + i1*cmx1) >> 10;

        }
    }
    /*----------------------------------------------------------*/

    vb_free (x0);
    vb_free (x1);
    vb_free (y0);
    vb_free (y1);

    vb_free (mult_x0);
    vb_free (mult_x1);
    vb_free (mult_y0);
    vb_free (mult_y1);

    return (VB_OK);
}
/*----------------------------------------------------------------------*/
int vb_zoom
/*----------------------------------------------------------------------*/
(
double		scale,
unsigned char	*row[],
int		n_rows,
int		n_cols,
int		bpp,
unsigned char	*small_row[],
int		ns_rows,
int		ns_cols
)
/*----------------------------------------------------------------------*/
{
    int		    i,j,k,l,m;
    int		    j3;
    int		    R,G,B;
    int             n_fold;
    int             n_fold2;
    int		    r_off,c_off;
    int		    r_off1,c_off1;
    double	    r;
    int		    nr,nc;
    unsigned char   *block;
    unsigned char   **pic;
    double	    ratio;

    r = ceil(1.0/scale);
    n_fold = (int)r;
    n_fold2 = n_fold*n_fold;

    nr = ns_rows*n_fold;
    nc = ns_cols*n_fold;

    block = (unsigned char*)vb_malloc(nr*nc*bpp);
    if (block == NULL) return(VB_MEM);

    pic = (unsigned char**)vb_malloc ((sizeof(*pic))*nr);
    if (pic == NULL)
    {
      vb_free (block);
      return(VB_MEM);
    }

    for (i=0; i < nr; ++i)
	pic[i] = block + i*nc*bpp;

    if (bpp == 3)
    {
	ratio = (double)nr/(double)n_rows;
	i = vb_chg_resRGB (ratio,row,n_rows,n_cols,pic,nr,nc);
	if (i != VB_OK)
   {
      vb_free (block);
      vb_free (pic);
      return(i);
   }

        for (i=0; i < ns_rows; ++i)
        {
	    r_off = i*n_fold;
            for (j=0; j < ns_cols; ++j)
            {
		j3 = j*3;
		c_off = j3*n_fold;
		R=0;
		G=0;
		B=0;
                for (k=0; k < n_fold; ++k)
                {
                    for (l=0; l < n_fold; ++l)
                    {
			r_off1 = r_off+k;
			c_off1 = c_off+l*3;
                        R += *(pic[r_off1]+c_off1);
                        G += *(pic[r_off1]+c_off1+1);
                        B += *(pic[r_off1]+c_off1+2);
                    }
                }
                *(small_row[i]+j3)   = R/n_fold2;
                *(small_row[i]+j3+1) = G/n_fold2;
                *(small_row[i]+j3+2) = B/n_fold2;
            }
        }
    }
    else
    {
	ratio = (double)nr/(double)n_rows;
	i = vb_chg_res (ratio,row,n_rows,n_cols,pic,nr,nc);
	if (i != VB_OK)
   {
      vb_free (block);
      vb_free (pic);
      return(i);
   }

	for (i=0; i < ns_rows; ++i)
        {
	    r_off = n_fold*i;
            for (j=0; j < ns_cols; ++j) 
            {
		c_off = n_fold*j;
                m = 0;
                for (k=0; k < n_fold; ++k) 
                {
                    for (l=0; l < n_fold; ++l)
                    { 
                        m += *(pic[r_off+k]+c_off+l);
                    }
                }    
                *(small_row[i]+j) = m/n_fold2;
            }
	}
    }

    vb_free(pic);
    vb_free(block);

    return(VB_OK);
}
/*----------------------------------------------------------------------*/

