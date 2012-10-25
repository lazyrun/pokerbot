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
	//������������ �����
	void SwapRow (int r1, int r2);
	//������������� �������
	void Negate ();
	//������� ������� �������� ���������
	void SolveLinear (Matrix *vec);
    //��������� ������������
	double DotProduct (Matrix *m);
    
	Matrix & operator = (Matrix &);

};
