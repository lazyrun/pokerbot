
#include "gaussconv.h"
#define _USE_MATH_DEFINES 
#include <math.h>

CGaussConv::CGaussConv(void)
{
};

//------------------------------------------------------------------------------------
CGaussConv::~CGaussConv(void)
{
	delete mask;
};
//------------------------------------------------------------------------------------

//насчитанные значения одномерного гауссиана для заданного сигма
//все насчитанные значения хранятся в mask
//маска накладывается на исходное изображение
CGaussConv::CGaussConv(double sigma)
{
	//С удалением от центра пиксели оказывают уменьшающее влияние на свертку
	//и пиксели удаленные более чем на 3*сигма оказывают ничтожное влияние на результат
	//поэтому ограничимся 3*сигма пикселей в каждую сторону относительно центрального пикселя
	//для которого производится преобразование
	   
	int dim = 1 + 2 * ((int) (3.0 * sigma));
	//если нечетное то имеем центральный пиксель
	//если размерность четная то дополняем размерность 
	dim |= 1;
	mask = new ConvLinearMask(dim);

	double sigma2sq = 2 * sigma * sigma;
	double normalizeFactor = 1.0 / (sqrt (2.0 * M_PI) * sigma);

	for (int n = 0 ; n < dim ; ++n) 
	{
		int relPos = n - mask->Middle();

		double G = (relPos * relPos) / sigma2sq;
		G = exp (-G);
		G *= normalizeFactor;
		
		mask->setMask(n, G);
	}
};

//------------------------------------------------------------------------------------
double CGaussConv::CalculateConvolutionValue1D (double** src, int n, int p, int /*maxN*/, int maxP, Direction dir)
{
	double sum = 0.0;
   //признак выхода маски за границы рисунка
	bool isOut = false;
	double outBound = 0.0;	// значения маски которые оказались за границей

	for (int xw = 0 ; xw < mask->Count() ; ++xw) 
	{
		int curAbsP = xw - mask->Middle() + p;//абсолютная позиция пикселя с учетом маски
         //если с учетом маски вышли за границы рисунка
		if (curAbsP < 0 || curAbsP >= maxP)
		{
			isOut = true;
			outBound += mask->getMask(xw);
			continue;
		}

		if (dir == Vertical)
			sum += mask->getMask(xw) * src[n][curAbsP];
		else
			sum += mask->getMask(xw) * src[curAbsP][n];
		
	}
	//если часть маски вышла за границы корректировать результат
	//отношением in/out
	if (isOut)
		sum *= 1.0 / (1.0 - outBound);

	return sum;
};
//------------------------------------------------------------------------------------
void CGaussConv::Convolve1D (double **dest, double **src, int width, int height, Direction dir)
{
	int maxN;	
	int maxP;	

	if (dir == Vertical)
	{
		maxN = width;//XDim; 
		maxP = height;//YDim;
	} 
	else if (dir == Horizontal) 
	{
		maxN = height;//YDim;
		maxP = width;//XDim;
	} 

	//по всем строкам и столбцам матрицы пикселей
	for (int n = 0 ; n < maxN ; n++)
	{
		for (int p = 0 ; p < maxP ; ++p)
		{
			double val = CalculateConvolutionValue1D (src, n, p, maxN, maxP, dir);

			if (dir == Vertical)
			{
				dest[n][p] = (double)val;
			}
			else
			{
				   dest[p][n] = (double)val;
			}
		}
	 }
};
//------------------------------------------------------------------------------------
void CGaussConv::Convolve (double **img, double **dest, int width, int height)
{
	double **res2, *Bits;

	res2 = new double* [width];
	Bits = new double[width*height];
	for (int i=0; i < width; i++)
		res2[i] = Bits + i*height;


	Convolve1D (dest, img, width, height, Vertical);
	Convolve1D (res2, dest, width, height, Horizontal);

	//скопировать данные
	for (int i=0; i<width; i++)
		for (int j=0; j<height; j++)
			dest[i][j] = res2[i][j];

	delete [] res2;
	delete [] Bits;

};

//------------------------------------------------------------------------------------
