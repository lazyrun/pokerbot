#include "raster.h"
#include "enhance.h"

//----------------------------------------------------------------------------
CRaster::CRaster(void)
{
	xDim = 0;
	yDim = 0;
	values = NULL;
}
//----------------------------------------------------------------------------
CRaster::~CRaster(void)
{
	if (values != NULL)
	{
		delete [] values;
		delete [] Bits;
		values = NULL;
		Bits = NULL;
	}
}
//----------------------------------------------------------------------------
//Принимает значения 0-255 в сером цвете
CRaster::CRaster(unsigned char **values, int xDim, int yDim)
{
	this->xDim = xDim;
	this->yDim = yDim;
	
   this->values = new double* [xDim];
	this->Bits = new double[xDim*yDim];
	for (int i=0; i < xDim; i++)
		this->values[i] = this->Bits + i*yDim;
	
	for (int y=0; y<yDim; y++)
	{
      for (int x=0; x<xDim; x++)
		{
         this->values[x][y] = (double)values[x][y]/255.0;
		}
	}
}
//----------------------------------------------------------------------------
CRaster::CRaster(QImage *img)
{
	this->xDim = img->width();
	this->yDim = img->height();
	
   this->values = new double* [xDim];
	this->Bits = new double[xDim*yDim];
	for (int i=0; i < xDim; i++)
		this->values[i] = this->Bits + i*yDim;
	
	for (int y=0; y<yDim; y++)
	{
      for (int x=0; x<xDim; x++)
		{
         QRgb rgb = img->pixel(x, y);
         int gray = qGray(rgb);

         this->values[x][y] = (double)gray/255.0;
		}
	}
}
//----------------------------------------------------------------------------
CRaster::CRaster(int xDim, int yDim)
{
	this->xDim = xDim;
	this->yDim = yDim;
	if (values == NULL)
	{
		values = new double* [xDim];
		Bits = new double[xDim*yDim];
		for (int i=0; i < xDim; i++)
			values[i] = Bits + i*yDim;
	}
}
//----------------------------------------------------------------------------
CRaster::CRaster(const CRaster &copy)
{
	this->xDim = copy.xDim;
	this->yDim = copy.yDim;
//	if (this->values == NULL)
//	{
		this->values = new double* [this->xDim];
		this->Bits = new double[this->xDim*this->yDim];
		for (int i=0; i < this->xDim; i++)
			this->values[i] = this->Bits + i*this->yDim;
//	}
	
	for (int y=0; y < this->yDim; y++)
		for (int x=0; x < this->xDim; x++)
			this->values[x][y] = copy.values[x][y];

}
//----------------------------------------------------------------------------
CRaster & CRaster :: operator =(CRaster &rastr)
{
  if (this != &rastr)
   {
		if (xDim != rastr.xDim ||
	       yDim != rastr.yDim)
		   return *this;

	   for (int y=0; y < this->yDim; y++)
		{
			for (int x=0; x < this->xDim; x++)
			{
			   this->values[x][y] = rastr.values[x][y];
			}
		}
   }
	return *this;
}
//----------------------------------------------------------------------------
void CRaster::ScaleHalf()
{
	//CRaster *half = new CRaster(*this);
	//сделать копию
	double **src = new double* [xDim];
	double *BitsSrc = new double[xDim*yDim];
	for (int i=0; i < xDim; i++)
		src[i] = BitsSrc + i*yDim;

	for (int y = 0 ; y < yDim ; y++)
	{
		for (int x = 0 ; x < xDim ; x++) 
		{
			src[x][y] = values[x][y];
		}
	}

	xDim = xDim/2;
	yDim = yDim/2;
  
	for (int y = 0 ; y < yDim ; y++)
	{
		for (int x = 0 ; x < xDim ; x++) 
		{
			values[x][y] = src[2*x][2*y];
		}
	}
	
	delete [] src;
	delete [] BitsSrc;
}
//----------------------------------------------------------------------------
void CRaster::ScaleDouble(double ratio)
{
	//double ratio = 2.0;
  
	//передать в функцию из enhance.h
	unsigned char **ini_row;
	unsigned char *ini_rowBits;
	int n_ini_row, n_ini_col;
	 
	unsigned char **res_row;
	unsigned char *res_rowBits;
	int n_res_row, n_res_col;
 
	n_ini_row = yDim;
	n_ini_col = xDim;
	n_res_row = yDim*2;
	n_res_col = xDim*2;
 
	ini_row = new unsigned char* [n_ini_col];
	ini_rowBits = new unsigned char [n_ini_col*n_ini_row];
	for (int i=0; i < n_ini_col; i++)
		ini_row[i] = ini_rowBits + i*n_ini_row;

	res_row = new unsigned char* [n_res_col];
	res_rowBits = new unsigned char [n_res_col*n_res_row];
	for (int i=0; i < n_res_col; i++)
		res_row[i] = res_rowBits + i*n_res_row;

	for (int i=0; i<n_ini_col; i++)
	{
		for (int j=0; j<n_ini_row; j++)
		{
			ini_row[i][j] = (unsigned char)(values[i][j]*255.);
		}
	}

	vb_chg_res(ratio, ini_row, n_ini_col,n_ini_row, res_row, n_res_col, n_res_row );

	Redistributive(int(double(xDim)*ratio), int(double(yDim)*ratio));
 
	for (int y = 0 ; y < yDim ; y++)
	{
		for (int x = 0 ; x < xDim ; x++) 
		{
			values[x][y] = res_row[x][y]/255.;
		}
	}
	  
	//for (int i=0; i < n_ini_col; i++)
	//	delete [] *(ini_row+i);
	delete [] ini_row;
	delete [] ini_rowBits;
	//for (int i=0; i < n_res_col; i++)
	//	delete [] *(res_row+i);
	delete [] res_row;
	delete [] res_rowBits;
}
//----------------------------------------------------------------------------
void CRaster::Redistributive(int xDim, int yDim)
{
	if (values != NULL)
	{
		//for (int i=0; i < this->xDim; i++)
		//delete [] *(values+i);
		
		delete [] values;
		delete [] Bits;
		
	}
    
	this->xDim = xDim;
	this->yDim = yDim;	
	
	values = new double* [xDim];
	Bits = new double [xDim*yDim];
   for (int i=0; i < xDim; i++)
		values[i] = Bits + i*yDim;
}
