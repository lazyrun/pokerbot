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
//класс маски линейной свертки
class ConvLinearMask
{
protected:
	int dim;
	int middle;
	double *mask;
	double maskSum;
public:
	//количество элементов участвующих в свертке
	int Count(void) {return dim;}
	//середина 
	int Middle(void) {return middle;}
	//получить значение маски
	double getMask(int idx){return mask[idx];}
	//установить значение маски
	void setMask(int idx, double value){mask[idx] = value;}
	
	ConvLinearMask(){};
	//конструктор
	ConvLinearMask (int dims)
	{
		mask = new double[dims];
		dim = dims;
		middle = dim / 2;
	}
	//деструктор
   ~ConvLinearMask () { delete [] mask; }
};

