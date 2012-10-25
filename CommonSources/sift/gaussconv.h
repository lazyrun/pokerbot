#pragma once

class ConvLinearMask;
//---------------------------------------------------------------------------------------
class CGaussConv
{
	ConvLinearMask *mask;
public:
	CGaussConv(void);
   CGaussConv (double sigma);

	~CGaussConv(void);
    
   enum Direction 
	{
		Vertical,
		Horizontal
	};

	void Convolve (double **img, double **dest, int width, int height);

protected:
    double CalculateConvolutionValue1D (double** src, int n, int p, int maxN, int maxP, Direction dir);
    void Convolve1D (double **dest, double **src, int width, int height, Direction dir);
};
//---------------------------------------------------------------------------------------
//����� ����� �������� �������
class ConvLinearMask
{
protected:
	int dim;
	int middle;
	double *mask;
	double maskSum;
public:
	//���������� ��������� ����������� � �������
	int Count(void) {return dim;}
	//�������� 
	int Middle(void) {return middle;}
	//�������� �������� �����
	double getMask(int idx){return mask[idx];}
	//���������� �������� �����
	void setMask(int idx, double value){mask[idx] = value;}
	
	ConvLinearMask(){};
	//�����������
	ConvLinearMask (int dims)
	{
		mask = new double[dims];
		dim = dims;
		middle = dim / 2;
	}
	//����������
   ~ConvLinearMask () { delete [] mask; }
};

