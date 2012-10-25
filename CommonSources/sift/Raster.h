#pragma once
#include <QImage>

class CRaster
{
protected:
	double **values;
	double *Bits;
	int xDim, yDim;
public:
	CRaster(void);
	CRaster(int xDim, int yDim);
	CRaster(const CRaster &copy);
	
	CRaster::CRaster(unsigned char **values, int xDim, int yDim);
   CRaster::CRaster(QImage *);
	~CRaster(void);
   
	CRaster & operator = (CRaster &);
	CRaster & operator - (CRaster &);
    //inline functions
	void Set(int x, int y, double val){values[x][y] = val;};
	double Get(int x, int y){return values[x][y];};
	int Width(){return xDim;};	
	int Height(){return yDim;};	
    
	//уменьшить масштаб надвое
	void ScaleHalf();
	//увеличить масштаб
	void ScaleDouble(double ratio = 2.0);
	//перераспределить память
	void Redistributive(int xDim, int yDim);
	//возвратить адрес values
	double **PtrValues(){return values;};
};
