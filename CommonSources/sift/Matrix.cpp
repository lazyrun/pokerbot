#include "matrix.h"

#include <math.h>

Matrix::Matrix(int xDim_in, int yDim_in)
{
	xDim = xDim_in; yDim = yDim_in;

	values = new double* [yDim];
	Bits = new double [yDim*xDim];
	for (int i=0; i < yDim; i++)
		values[i] = Bits + i*xDim;
};
//---------------------------------------------------------------------------------------
Matrix::~Matrix(void)
{
	//for (int i=0; i < yDim; i++)
	//	delete [] *(values+i);
	 
	delete [] values;
	delete [] Bits;
};
//---------------------------------------------------------------------------------------
double Matrix::Get(int y, int x)
{
	if ((x < xDim) && (y < yDim))
	{
		return values[y][x];
	}
	return -1.0;
};
//---------------------------------------------------------------------------------------
void Matrix::Set(int y, int x, double val)
{
	if ((x < xDim) && (y < yDim))
	{
		values[y][x] = val;
	}
};
//---------------------------------------------------------------------------------------
void Matrix::SwapRow (int r1, int r2)
{
	if (r1 == r2)
		return;

	for (int x = 0 ; x < xDim ; ++x)
	{
		double temp = values[r1][x];
		values[r1][x] = values[r2][x];
		values[r2][x] = temp;
	}
};
//---------------------------------------------------------------------------------------
void Matrix::Negate ()
{
	for (int y = 0 ; y < yDim ; ++y) 
	{
		for (int x = 0 ; x < xDim ; ++x) 
		{
			values[y][x] = -values[y][x];
		}
	}
};
//---------------------------------------------------------------------------------------
//решение системы линейных уравнений
void Matrix::SolveLinear (Matrix *vec)
{
	if ((xDim != yDim) || (yDim != vec->yDim))
		return;

	// Forward elimination with partial pivoting
	for (int y = 0 ; y < (yDim - 1) ; ++y)
	{
		// Searching for the largest pivot (to get "multipliers < 1.0 to
		// minimize round-off errors")
		int yMaxIndex = y;
		double yMaxValue = abs (values[y][y]);

		for (int py = y ; py < yDim ; ++py) 
		{
			if (abs (values[py][y]) > yMaxValue) 
			{
				yMaxValue = abs (values[py][y]);
				yMaxIndex = py;
			}
		}

		// if a larger row has been found, swap with the current one
		SwapRow (y, yMaxIndex);
		vec->SwapRow (y, yMaxIndex);

		// Now do the elimination left of the diagonal
		for (int py = y + 1 ; py < yDim ; ++py) 
		{
			// always <= 1.0
			double elimMul = values[py][y] / values[y][y];

			for (int x = 0 ; x < xDim ; ++x)
				values[py][x] -= elimMul * values[y][x];

			//vec.Set(0, py, vec.Get(0, py) - elimMul * vec.Get(0, y));
			vec->values[py][0] -= elimMul * vec->values[y][0];
		}
	}

	// Back substitution (обратная подстановка)
	for (int y = yDim - 1 ; y >= 0 ; --y) 
	{
		double solY = vec->values[y][0];

		for (int x = xDim - 1 ; x > y ; --x)
			solY -= values[y][x] * vec->values[x][0];

		vec->values[y][0] =  solY / values[y][y];
	}
   return;	
};
//---------------------------------------------------------------------------------------
//скалярное произведение векторов
double Matrix::DotProduct (Matrix *m)
{
	if ((yDim != m->yDim) || (xDim != 1) || (m->xDim != 1))
		return -1.0;	

	double sum = 0.0;

	for (int y = 0 ; y < yDim ; ++y)
		sum += values[y][0] * m->values[y][0];

	return sum;
};
//---------------------------------------------------------------------------------------
Matrix & Matrix :: operator =(Matrix &matr)
{
	if (this != &matr)
	{
		xDim = matr.xDim;
		yDim = matr.yDim;
		for (int y=0; y<yDim; y++)
		{
			for (int x=0;x<xDim; x++)
			{
				values[y][x] = matr.values[y][x];
			}
		}
	}
	return *this;
};