#pragma once

class Matrix
{
protected:
	double **values;
	double *Bits;
	int xDim, yDim;

public:
	Matrix(int xDim, int yDim);
	~Matrix(void);

	double Get(int y, int x);
	void Set(int y, int x, double val);
	//перестановка строк
	void SwapRow (int r1, int r2);
	//отрицательная матрица
	void Negate ();
	//решение системы линейных уравнений
	void SolveLinear (Matrix *vec);
    //скалярное произведение
	double DotProduct (Matrix *m);
    
	Matrix & operator = (Matrix &);

};
