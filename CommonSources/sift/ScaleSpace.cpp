#include "scalespace.h"
//#include <cstdio>

#define SQR(x) (x*x)

COctavePyramid::COctavePyramid()
{
};
//---------------------------------------------------------------------------------------
COctavePyramid::~COctavePyramid()
{
};
//---------------------------------------------------------------------------------------
void COctavePyramid::BuildOctaves (CRaster *source, 
		int levelsPerOctave, double octaveSigma, int minSize)
{
	CRaster prev(*source);
	 
	double scale = 0.5;
	octaves.reserve(10);
	
	while (&prev != NULL && prev.Width() >= minSize && prev.Height() >= minSize) 
	{
		CScaleSpace dsp;
		
		//������� ��������� � �� ������� ��� ������� ������
		
		dsp.BuildGaussianMaps (&prev, scale, levelsPerOctave, octaveSigma);
		dsp.BuildDiffMaps();

		//��������� ����������� ������������ �������� � ������ �����
		octaves.push_back(dsp);
		
		//�������� ��������� ��������� ��������
		for (int y=0; y < prev.Height(); y++)
		{
			for (int x=0; x < prev.Width(); x++)
			{
				prev.Set(x, y, dsp.imgScaledD[dsp.CimgScaled - 2][x][y]);
			}
		}
		
		//��������� ������� �������� ������
		prev.ScaleHalf();
		
		//������������� ������� 1 � 2�
		scale *= 2.0;
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::FindPeaks ()
{
	double **current, **above, **below;
	for (int level =1; level < Cspaces-1; ++level)
	{
		current = spacesD[level];
		below = spacesD[level - 1];
		above = spacesD[level + 1];
		FindPeaksThreeLevel (below, current, above,	level);
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::FindPeaksThreeLevel (double** below, double** current,
		double** above, int curLev)
{
	peaks.reserve(Yspaces*Xspaces);
	for (int y = 1 ; y < (Yspaces - 1) ; ++y) 
	{
		for (int x = 1 ; x < (Xspaces - 1) ; ++x)
		{
			bool cIsMax = true;
			bool cIsMin = true;

			double c = current[x][y];	// �������� ������

			if (fabs(c) <= DOG_THRESH)
				continue;

			CheckMinMax (current, c, x, y, cIsMin, cIsMax, true);
			CheckMinMax (below, c, x, y, cIsMin, cIsMax, false);
			CheckMinMax (above, c, x, y, cIsMin, cIsMax, false);
			if (cIsMin == false && cIsMax == false)
				continue;
			
			CScalePoint point(x, y, curLev);
			peaks.push_back(point);

		}
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::CheckMinMax (double **layer, double c, int x, int y,
		 bool &IsMin, bool &IsMax, bool cLayer)
{
		//������ �� ����� ���������� ���������� �������
	if (IsMin == true) 
	{
	if (layer[x - 1][y - 1] <= c ||
	    layer[x][y - 1] <= c ||
		 layer[x + 1][y - 1] <= c ||
		 layer[x - 1][ y] <= c ||
		 // ����� ������ < ������ <=
		 //�������� �� ������� ������, ��������� ����� ��� ���
		 (cLayer ? false : (layer[x][y] < c)) ||
		 layer[x + 1][y] <= c ||
		 layer[x - 1][y + 1] <= c ||
		 layer[x][y + 1] <= c ||
		 layer[x + 1][y + 1] <= c)
		 IsMin = false;
		}
		if (IsMax == true) 
		{
		if (layer[x - 1][y - 1] >= c ||
			layer[x][y - 1] >= c ||
			layer[x + 1][y - 1] >= c ||
			layer[x - 1][y] >= c ||
			// ����� ������  > ������ >=
			(cLayer ? false : (layer[x][y] > c)) ||
			layer[x + 1][y] >= c ||
			layer[x - 1][y + 1] >= c ||
			layer[x][y + 1] >= c ||
			layer[x + 1][y + 1] >= c)
			IsMax = false;
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::FilterAndLocalizePeaks ()
{
	//filtered.reserve((int)peaks.size());
	//for (int i=0; i < (int)peaks.size(); i++)
	//{
	//	//��������� �� ������ ���������
	//	if (LocalizeHasLowContrast(&peaks[i], RELOCATION_MAXIMUM))
	//		continue;
	//	//��������� �� ��������� ������� �������
	//	if (CheckEdgeResponses(spacesD[peaks[i].level], peaks[i].x, peaks[i].y))
	//		continue; 	
	//	//���� �������� ����� �� ���������� ������ ������ 0.5 �� ���������
	//	//���������� ��� ��� ����� ���������, �.�. �������� � ������ ���������
	//	if (peaks[i].local.scaleAdjust > SCALE_ADJUST_THRESH)
	//		continue;
	//	//���� ������ 0.03 
	//	if (fabs(peaks[i].local.dValue) <= DVALUE_LOW_THRESH)
	//		continue;
 //   
	//	filtered.push_back(peaks[i]);
	//}
   filtered = peaks;
};
//---------------------------------------------------------------------------------------
bool CScaleSpace::CheckEdgeResponses (double** space, int x, int y)
{
	double D_xx, D_yy, D_xy;
	double r = EDGE_RATIO;

	// ���������� ������� Hessian [ D_xx, D_xy ; D_xy , D_yy ]
	D_xx = space[x + 1][y] + space[x - 1][y] - 2.0 * space[x][y];
	D_yy = space[x][y + 1] + space[x][y - 1] - 2.0 * space[x][y];
	D_xy = 0.25 * ((space[x + 1][y + 1] - space[x + 1][y - 1]) -
	      (space[x - 1][y + 1] - space[x - 1][y - 1]));

	double TrHsq = D_xx + D_yy;
	TrHsq *= TrHsq;
	double DetH = D_xx * D_yy - (D_xy * D_xy);

	double r1sq = (r + 1.0);
	r1sq *= r1sq;
	
	if ((TrHsq / DetH) < (r1sq / r))
  	  return false;
	
	return true;
};
//---------------------------------------------------------------------------------------
bool CScaleSpace::LocalizeHasLowContrast (CScalePoint *point, int steps)
{
	bool needToAdjust = true;
	int adjusted = steps;
  
	double **spaceXY = spacesD[point->level];
	while (needToAdjust)
	{
		int x = point->x;
		int y = point->y;
		//���� ����� ������������ �� ������� ��� ������ ���������� ����������� ������������
		//�� �� ��������� �� � ����� ���������
		//��� ��������� ��� ���������������
		if ((point->level <= 0) || (point->level >= (Cspaces - 1)))
			return true;
	    
		//���� ����� ����������� �� ������� �����������, �� ��������� ��
		//��� ����������� �� �����
		if ((x <= 0) || (x >= (Xspaces - 1)))
			return true;
		if ((y <= 0) || (y >= (Yspaces - 1)))
			return true;
	   
		double dp;
		Matrix *adj=new Matrix(1, 3);

		GetAdjustment (*point, &dp, adj);
		//���������� �� ������������
		double adjS = adj->Get(0, 0);
		double adjY = adj->Get(1, 0);
		double adjX = adj->Get(2, 0);

		delete adj;
		if((abs(adjX) > 0.5) || (abs(adjY) > 0.5))
		{
			//���� �������� x ������ ��� 0.5 � ����� �����������,
			//�� ��� ��������, ��� ��������� ����� ����� � ������ ������� �����

			//���� �� ����� ��������� �� ��������� �����, �� ������ ����� ����� ���������� (������������)
			//� ������������� �����������
			//������ ����� ����� ����� ������ ������ ������������� �����
			if (adjusted == 0)
			  return true;

			adjusted -= 1;
			//��� �� ������ ��� � 1 �������
			//���� ���������� ������� �������, �� ����� ����� ���������� - � ��� �����������
			//������ ����� ����� ����� ������������� ����������
			double distSq = adjX * adjX + adjY * adjY;
			if (distSq > 2.0)
				return true;
		     
			//������� ��� � ��� �����
			point->x = (int) (point->x + adjX + 0.5);
			point->y = (int) (point->y + adjY + 0.5);
			 	 
			continue;
		}
		//������������ ���������
  		CPointLocalInformation *Local = new CPointLocalInformation(adjS, adjX, adjY);
		//����������� ����� ������ ����������� ����������
		//����������� ������� DoG �� ���������� 
		//���� ��� ������ 0,008 �� � ��� ������ �������� � ����� ����� �����������
		Local->dValue = spaceXY[point->x][point->y] + 0.5 * dp;
		point->local = *Local;
		
		delete Local;
		needToAdjust = false;	

	}//while (needToAdjust)

	return false;
};
//---------------------------------------------------------------------------------------

void CScaleSpace::GetAdjustment (CScalePoint point, double* dp, Matrix *b)
{
	*dp = 0.0;

	if (!((point.level <= 0) || (point.level >= (Cspaces - 1))))
	{
		//����������� ��� ����������� �� �����
		double **currentXY, **belowXY, **aboveXY;
		belowXY= spacesD[point.level - 1];
		currentXY = spacesD[point.level];
		aboveXY = spacesD[point.level + 1];

		int x, y;
		x = point.x;
		y = point.y;

		Matrix *H = new Matrix(3, 3);

		H->Set(0, 0, belowXY[x][y] - 2 * currentXY[x][y] + aboveXY[x][y]);

		H->Set(0, 1, 0.25 * (aboveXY[x][y + 1] - aboveXY[x][y - 1] -
				(belowXY[x][y + 1] - belowXY[x][y - 1])));
		H->Set(1, 0, H->Get(0, 1));
		 
		H->Set(0, 2, 0.25 * (aboveXY[x + 1][y] - aboveXY[x - 1][y] -
				(belowXY[x + 1][y] - belowXY[x - 1][y])));
		H->Set(2, 0, H->Get(0, 2)); 
		 
		H->Set(1, 1, currentXY[x][y - 1] - 2 * currentXY[x][y] + currentXY[x][y + 1]);
		 
		H->Set(1, 2, 0.25 * (currentXY[x + 1][y + 1] - currentXY[x - 1][y + 1] -
				(currentXY[x + 1][y - 1] - currentXY[x - 1][y - 1])));
		H->Set(2, 1, H->Get(1, 2));

		H->Set(2, 2, currentXY[x - 1][y] - 2 * currentXY[x][y] + currentXY[x + 1][y]);

		Matrix *d = new Matrix (1, 3);
		d->Set(0, 0, 0.5 * (aboveXY[x][y] - belowXY[x][y]));
		d->Set(1, 0, 0.5 * (currentXY[x][y + 1] - currentXY[x][y - 1]));
		d->Set(2, 0,  0.5 * (currentXY[x + 1][y] - currentXY[x - 1][y]));

		//Matrix *b = new Matrix (1, 3);
		b->Set(0, 0, d->Get(0, 0));
		b->Set(1, 0, d->Get(1, 0));
		b->Set(2, 0, d->Get(2, 0));

		b->Negate ();

		// Solve: H x = b
		H->SolveLinear(b);
		 
		*dp = b->DotProduct(d);

		delete H;
		delete d;
		return;
		//return *b;//x^
	}
};
//---------------------------------------------------------------------------------------
CScaleSpace::CScaleSpace(void)
{
};
//---------------------------------------------------------------------------------------
CScaleSpace::~CScaleSpace(void)
{
	ClearImgScaledD();
	ClearSpacesD();
};
//---------------------------------------------------------------------------------------
CScaleSpace::CScaleSpace(const CScaleSpace &copying)
{
	basePixScale = copying.basePixScale;
	//����������� ���� � ������������� ����
	peaks = copying.peaks;
	filtered = copying.filtered;
	
	//����������� ��������������� ��������
	XimgScaled = copying.XimgScaled;
	YimgScaled = copying.YimgScaled;
	CimgScaled = copying.CimgScaled;

	Xspaces = copying.Xspaces;
	Yspaces = copying.Yspaces;
	Cspaces = copying.Cspaces;

	imgScaledD = new double**[CimgScaled];
	BitsImgScaledD = new double [CimgScaled*XimgScaled*YimgScaled];
	
	for (int k=0; k<CimgScaled; k++)
	{
		imgScaledD[k] = new double *[XimgScaled];
		for (int i=0; i<XimgScaled; i++)
		{
			imgScaledD[k][i] = BitsImgScaledD + k*XimgScaled*YimgScaled + i*YimgScaled;
		}
	}

	for (int i=0; i<CimgScaled; i++)
	 {
		 for (int j=0; j<YimgScaled; j++)
		 {
			 for (int k=0; k<XimgScaled; k++)
			 {
				 imgScaledD[i][k][j] = copying.imgScaledD[i][k][j];
			 }
		 }
	 }

	spacesD = new double**[Cspaces];
	BitsSpacesD = new double [Cspaces*Xspaces*Yspaces];
	
	for (int k=0; k<Cspaces; k++)
	{
		spacesD[k] = new double *[Xspaces];
		for (int i=0; i<Xspaces; i++)
		{
			spacesD[k][i] = BitsSpacesD + k*Xspaces*Yspaces + i*Yspaces;
		}
	}

	for (int i=0; i<Cspaces; i++)
	{
		 for (int j=0; j<Yspaces; j++)
		 {
			 for (int k=0; k<Xspaces; k++)
			 {
				 spacesD[i][k][j] = copying.spacesD[i][k][j];
			 }
		 }
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::BuildDiffMaps()
{
	//��������� ������
	Cspaces = CimgScaled - 1;
	Xspaces = XimgScaled;
	Yspaces = YimgScaled;

	spacesD = new double**[Cspaces];
	BitsSpacesD = new double [Cspaces*Xspaces*Yspaces];
	
	for (int k=0; k<Cspaces; k++)
	{
		spacesD[k] = new double *[Xspaces];
		for (int i=0; i<Xspaces; i++)
		{
			spacesD[k][i] = BitsSpacesD + k*Xspaces*Yspaces + i*Yspaces;
		}
	}

	for (int sn = 0 ; sn < Cspaces; sn++) 
	{
		for (int y = 0; y < Yspaces; y++) 
		{
			for (int x = 0; x < Xspaces; x++) 
			{
				spacesD[sn][x][y] = imgScaledD[sn+1][x][y] - imgScaledD[sn][x][y];
			}
		}
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::BuildGaussianMaps(CRaster *first, double firstScale,
									int scales, double octaveSigma)
{
	/*
	first - �������� �����������
	firstScale - ��������� ������� �������� (����� ���� �������� ��� ��������, ���� = 1)
	�������� - ���� ����������� ������� �������
	�������� - ���� ������� ��������� (����� ��������)
	scales - ������� ������ ���������� DoG ���� ������� (s)
	octaveSigma - ����� ������ (1.6) ����������� �������� (������������� � ���������� *2^1/s)
	-------------------------------------------------
	��������� ������ - ����������� ������� imgScaled
	*/
	//��������� s+1 �������� ����������� ��� �������� s ����
	//� ��������� ���� ����������� ��� �������� � ���������
	//����� s+3
	
	//��������� ������
	CimgScaled = scales + 3;
	XimgScaled = first->Width();
	YimgScaled = first->Height();

	imgScaledD = new double**[CimgScaled];
	BitsImgScaledD = new double [CimgScaled*XimgScaled*YimgScaled];
	
	for (int k=0; k<CimgScaled; k++)
	{
		imgScaledD[k] = new double *[XimgScaled];
		for (int i=0; i<XimgScaled; i++)
		{
			imgScaledD[k][i] = BitsImgScaledD + k*XimgScaled*YimgScaled + i*YimgScaled;
		}
	}
		
 	basePixScale = firstScale;
	
	double **prev;//���������� ��������
	prev = new double*[XimgScaled];
	double *BitsPrev = new double[XimgScaled * YimgScaled];
	for (int j=0; j < XimgScaled; j++)
		prev[j] = BitsPrev + j*YimgScaled;

	for (int y=0; y < YimgScaled; y++)
	{
		for (int x=0; x < XimgScaled; x++)
		{
			prev[x][y] = first->Get(x, y);
			imgScaledD[0][x][y] = prev[x][y];
		}
	}
	double w = octaveSigma;
	//�����������
	double kTerm = 0.75;//1.0;
	
	for (int scI = 1 ; scI < CimgScaled ; scI++) 
	{
		CGaussConv *gauss = new CGaussConv (w * kTerm);
		//������� ����� ������ �������� ������ ��� ���
		//imgScaled[scI].CreateCopy(&prev, false);
		//���������� ���������� � prev � �������� ��������� � imgScaled[scI]
		gauss->Convolve(prev, imgScaledD[scI], XimgScaled, YimgScaled);
		//� prev ���������� ��������� ��������
		for (int y=0; y < YimgScaled; y++)
		{
			for (int x=0; x < XimgScaled; x++)
			{
				prev[x][y] = imgScaledD[scI][x][y];
			}
		}
		
		//����� - ����������� ��������- �������������
		w *= SToK (scales);
		delete gauss;
	}
	//�������� prev
	 delete [] BitsPrev;
	 delete [] prev;
};
//---------------------------------------------------------------------------------------
void CScaleSpace::ClearImgScaledD(void)
{
	int xDim, yDim;
	yDim = YimgScaled;
	xDim = XimgScaled;

	if ((imgScaledD != NULL) && (BitsImgScaledD != NULL))  
	{
		for (int i=0; i < CimgScaled; i++)
			delete [] imgScaledD[i];
		
		delete [] imgScaledD;
		delete [] BitsImgScaledD;
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::ClearSpacesD(void)
{
	int xDim, yDim;
	yDim = Yspaces;
	xDim = Xspaces;
	if (spacesD != NULL)
	{
		for (int i=0; i < Cspaces; i++)
		{
			delete [] spacesD[i];
		}
		delete [] spacesD;
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::AllocMagnitudes()
{
    //��������� ������
	int xDim, yDim;
	yDim = YimgScaled;
	xDim = XimgScaled;

	magnitudes = new double**[CimgScaled-1];
	BitsMagnitudes = new double [(CimgScaled-1)*xDim*yDim];
	for (int i=0; i < CimgScaled-1; i++)
	 {
		magnitudes[i] = new double*[xDim];
		for (int j=0; j < xDim; j++)
		{
			magnitudes[i][j] = BitsMagnitudes + i*xDim*yDim + j*yDim;
		}
	 }

	//�����������
	 Cmagdir = CimgScaled-1;
	 Xmagdir = xDim;
	 Ymagdir = yDim;
};
//---------------------------------------------------------------------------------------
void CScaleSpace::AllocDirections()
{
   //��������� ������
	int xDim, yDim;
	yDim = YimgScaled;
	xDim = XimgScaled;

	directions = new double**[CimgScaled-1];
	BitsDirections = new double [(CimgScaled-1)*xDim*yDim];
	for (int i=0; i < CimgScaled-1; i++)
	 {
		directions[i] = new double*[xDim];
		for (int j=0; j < xDim; j++)
		{
			directions[i][j] = BitsDirections + i*xDim*yDim + j*yDim;
		}
	 }
	//�����������
	 Cmagdir = CimgScaled-1;
	 Xmagdir = xDim;
	 Ymagdir = yDim;
};
//---------------------------------------------------------------------------------------
void CScaleSpace::ClearMagnitudes(void)
{
	int xDim, yDim;
	yDim = YimgScaled;
	xDim = XimgScaled;
	if (magnitudes != NULL) 
	{
		for (int i=0; i < CimgScaled-1; i++)
		{
			//for (int j=0; j < xDim; j++)
			//	delete [] magnitudes[i][j];
			
			delete [] magnitudes[i];
		}
		delete [] magnitudes;
		delete [] BitsMagnitudes;
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::ClearDirections(void)
{
	int xDim, yDim;
	yDim = YimgScaled;
	xDim = XimgScaled;
	if (directions != NULL)
	{
		for (int i=0; i < CimgScaled-1; i++)
		{
			//for (int j=0; j < xDim; j++)
			//	delete [] directions[i][j];
			
			delete [] directions[i];
		}
		delete [] directions;
		delete [] BitsDirections;
	}
};
//---------------------------------------------------------------------------------------
void CScaleSpace::CreateMagnitudesAndDirections()
{
	//�� ��������� ������ � ��������� ��������� ��� ���������������
	AllocMagnitudes();
	AllocDirections();

	//�������� ������� �� ����� �.�. �� ����� ��� ��� ��������� ���������
	for (int s = 1 ; s < Cmagdir ; ++s) 
	{
		for (int x = 1 ; x < (Xmagdir - 1) ; ++x) 
		{
			for (int y = 1 ; y < (Ymagdir - 1) ; ++y) 
			{
				// �������� ��������� m
				magnitudes[s][x][y] = sqrt (
					pow (imgScaledD[s][x + 1][y] -
					imgScaledD[s][x - 1][y], 2.0) +
					pow (imgScaledD[s][x][y + 1] -
					imgScaledD[s][x][y - 1], 2.0));

				// ����������� ��������� theta
				directions[s][x][y] = atan2 (
					imgScaledD[s][x][y + 1] - imgScaledD[s][x][y - 1],
					imgScaledD[s][x + 1][y] - imgScaledD[s][x - 1][y]);
			}
		}
	}
};
//������� �������� ����� �� ������ �������������� �����
//---------------------------------------------------------------------------------------
TVKeypoints CScaleSpace::GenerateKeypoints()
{
	TVKeypoints mainkp;
	mainkp.reserve(int(filtered.size()));
	for (int i=0; i < int(filtered.size()); i++) 
	{
		//� ���������� ������ ����������� ������ keypoints ���� CKeypoint
		//��� ����� ����� ScalePoint ����� ���� ���� ��� ��������� 
		//�������� ����� (������� 80%) ���  ������ TVKeypoints
		TVKeypoints keypoints = GenerateKeypointSingle(filtered[i]);
		//��� ��������� �������� ����� ������������ ��������� 4*4*8
		TVKeypoints kphasFV = CreateDescriptors(keypoints, filtered[i].level);

		for (int i=0; i<int(kphasFV.size()); i++)
		{
			if (kphasFV[i].hasFV)
			{
				kphasFV[i].x *= kphasFV[i].imgScale;
				kphasFV[i].y *= kphasFV[i].imgScale;
				kphasFV[i].scale *= kphasFV[i].imgScale;

				mainkp.push_back(kphasFV[i]);
			}
		}
	}
	return mainkp;	
};
//---------------------------------------------------------------------------------------
TVKeypoints CScaleSpace::CreateDescriptors(TVKeypoints kp, int level)
{
/* 
  keypoints - ����� ��� ������� ��������� ���������
  level - ������� � ������������ ���������, �������� ����������� �������� �����
  nominalScaleFactor - �����������, ������������ ���������� ������������ �������� � ����������� �������� �����
  descDim - ����������� ������� 4*4
  directionCount - ���������� ����������
  fvGradThresh - ��������� ��������
*/
	double nominalScaleFactor = 4.0;
	int descDim = 4;
	int directionCount = 8;
	double fvGradThresh = 0.2;

	if (int(kp.size()) <= 0)
		return kp;

	nominalScaleFactor *= kp[0].scale;//������� �������� �����
	
	//������, ����������� ������� ����������� descDim
	int radius = (int) (((descDim + 1.0) / 2) *
		sqrt (2.0) * nominalScaleFactor + 0.5);
	
	//���������� �������� �����
	TVKeypoints survivors;
	survivors.reserve(int(kp.size()));
	// Lowe04, page 15 "A Gaussian weighting function with
	// sigma equal to one half the width of the descriptor window is
	// used"
	double dDim05 = ((double) descDim) / 2.0;
	double sigma2Sq = 2.0 * dDim05 * dDim05;
	
	for (int i=0; i < int(kp.size()); i++)
	 {
		double angle = -kp[i].orientation;
		kp[i].hasFV = 1;
		
		for (int y = -radius ; y < radius ; y++)
		 {
			 for (int x = -radius ; x < radius ; x++) 
			 {
				 // Rotate and scale
				//����������� �������� x, y � ������� ���������, ���������� �� ���� angle
				double yR = sin (angle) * x +
							cos (angle) * y;
				double xR = cos (angle) * x -
							sin (angle) * y;
                //� �������������� 
				yR /= nominalScaleFactor;
				xR /= nominalScaleFactor;

				// ������������� (xR, yR) �������� � ��������
				// (- descDim/2 - 0.5 ; -descDim/2 - 0.5) 
				//    (descDim/2 + 0.5 ; descDim/2 + 0.5),
				
				if (yR >= (dDim05 + 0.5) || xR >= (dDim05 + 0.5) ||
					xR <= -(dDim05 + 0.5) || yR <= -(dDim05 + 0.5))
				 continue;

				int currentX = (int) (x + kp[i].x + 0.5);
				int currentY = (int) (y + kp[i].y + 0.5);
				if (currentX < 1 || currentX >= (Xmagdir - 1) ||
					currentY < 1 || currentY >= (Ymagdir - 1))
			      continue;
 
				double magW = exp (-(xR * xR + yR * yR) / sigma2Sq) *
					magnitudes[level][currentX][currentY];

				// ���������� � ��������� (-1.0, -1.0)-(dDim05 + 1.0, dDim05 + 1.0)
				yR += dDim05 - 0.5;
				xR += dDim05 - 0.5;
				
				int xIdx[2]={0};
				int yIdx[2]={0};
				int dirIdx[2]={0};
				double xWeight[2]={0.0};
				double yWeight[2]={0.0};
				double dirWeight[2]={0.0};

				if (xR >= 0)
				{
					xIdx[0] = (int) xR;
					xWeight[0] = (1.0 - (xR - xIdx[0]));
				}
				if (yR >= 0) 
				{
					yIdx[0] = (int) yR;
					yWeight[0] = (1.0 - (yR - yIdx[0]));
				}

				if (xR < (descDim - 1)) 
				{
					xIdx[1] = (int) (xR + 1.0);
					xWeight[1] = (1.0 - (xIdx[1] - xR));
				}
				if (yR < (descDim - 1)) 
				{
					yIdx[1] = (int) (yR + 1.0);
					yWeight[1] = (1.0 - (yIdx[1] - yR));
				}

				//������� ����������� ��������� �� ���������� �������� �����
				//�������� � ��������� [-pi ; pi]
				double dir = directions[level][currentX][currentY] - kp[i].orientation;
				if (dir <= M_PI)
					dir += M_PI;
				if (dir > M_PI)
					dir -= M_PI;
				//��������� �� 8 �������� �����������
				//�������� � ����� ������� �������� ����������� � ���� �����
				double idxDir = (dir * directionCount) /
					(2.0 * M_PI);
				if (idxDir < 0.0)
					idxDir += directionCount;
				dirIdx[0] = (int) idxDir;
				dirIdx[1] = (dirIdx[0] + 1) % directionCount;
				dirWeight[0] = 1.0 - (idxDir - dirIdx[0]);
				dirWeight[1] = idxDir - dirIdx[0];
		
				for (int iy = 0 ; iy < 2 ; iy++) 
				{
					for (int ix = 0 ; ix < 2 ; ix++) 
					{
						for (int id = 0 ; id < 2 ; id++) 
				 		{
							double fvprev = kp[i].FVGet(xIdx[ix], yIdx[iy], dirIdx[id]);
							kp[i].FVSet (xIdx[ix], yIdx[iy], dirIdx[id],
				  							fvprev +
											xWeight[ix] * yWeight[iy] * dirWeight[id] * magW);
						}
					}
				}

			 }//for (int x = -radius ; x < radius ; ++x) 
		 }//for (int y = -radius ; y < radius ; y++)

		 //������������ � ���������� FV
		 ThresholdAndNormalizeFV (&kp[i], fvGradThresh);
		
		 survivors.push_back(kp[i]);

	 }//for (int i=0; i < int(kp.size()); i++)

 return survivors;
};

//---------------------------------------------------------------------------------------
TVKeypoints CScaleSpace::GenerateKeypointSingle(CScalePoint point)
{
	TVKeypoints keypoints;
	double kpSigmaInScale = OCTAVE_SIGMA *
			pow (2.0, (point.level + point.local.scaleAdjust) / COUNT_SCALES);
 	// Lowe04, page 13, "gaussian-weighted circular window with a sigma 1.5
	// times that of the scale of the keypoint".
	double sigma = 1.5 * kpSigmaInScale;
    //�����������, ������� �� �������������
	//���� ��� ��������� �� ���������� ����� ���������
   int radius = int(3.0 * sigma / 2.0 + 0.5);
	int radiusSq = radius * radius;
	
	double **magnitude = magnitudes[point.level];
	double **direction = directions[point.level];

	//��� ��� ����� ����� ������ ����� � ��������
    //������� (�������������) ������ ����� �� �������� ������� �����
	int xMin = max (point.x - radius, 1);
	int xMax = min (point.x + radius, Xmagdir - 1);
	int yMin = max (point.y - radius, 1);
	int yMax = min (point.y + radius, Ymagdir - 1);

	// G(x) = e^{-{x^2}/{2*sigma^2}}
	//���������� ������������
	double gaussianSigmaFactor = 2.0 * sigma * sigma;
    //������� �����������
	//36 �������� �� 10 ��������
	int binCount = 36;
	double *bins;
	bins = new double[binCount];
    for (int i=0; i<binCount; i++) bins[i] = 0.0;
	//������ ����������� ����������
	for (int x = xMin; x < xMax; x++) 
	{
		for (int y = yMin; y < yMax; y++) 
	   {
		   //��������� ������� ������������� � �����
			int relX = x - point.x;
			int relY = y - point.y;
			if (IsInCircle (relX, relY, radiusSq) == false)
				continue;
			//��������� ��� ���������
			double gaussianWeight = exp	(- ((relX * relX + relY * relY) / gaussianSigmaFactor));

			//�������� ����������� � ������� �����������
			int binIdx = FindClosestBin (binCount, direction[x][y]);
			//������� ��������� ����������� � ������� 
			//����������� ���������� � ����� ���������
			bins[binIdx] += magnitude[x][y] * gaussianWeight;
		}
	}

	//���� ������������ �������� � ����������� ����������
	double maxGrad = 0.0;
	int maxBin = 0;
	for (int b = 0; b < binCount; b++) 
	{
		if (bins[b] > maxGrad) 
		{
			maxGrad = bins[b];
			maxBin = b;
		}
	}
	//���������� �������������� �������� �����
    
	bool *binIsKeypoint = new bool[binCount];
	for (int b = 0 ; b < binCount; b++) 
	{
		binIsKeypoint[b] = false;

		// ������������ ��� �������� �������� ������
		if (b == maxBin) 
		{
			binIsKeypoint[b] = true;
				continue;
		}
        //�� ���������� ��������
		if (bins[b] < (KEYPOINT_THRESH * maxGrad))
			continue;
        //������� ������ ���� ��������� �����
		int leftI;
		if (b == 0) 
		  leftI = (binCount - 1);
		else
		  leftI = (b - 1);

		int rightI = (b + 1) % binCount;
		if ((bins[b] <= bins[leftI]) || (bins[b] <= bins[rightI]))
			continue;

		binIsKeypoint[b] = true;
	}
	// ��� �������� ����� �������� � binIsKeypoint

	//������� ������� ����������� � ������������ ������������
	//�������� ���������� �������� �����

	double oneBinRad = (2.0 * M_PI) / binCount;//10 ��������

	keypoints.reserve(binCount);
	for (int b = 0 ; b < binCount ; b++) 
	{
		if (binIsKeypoint[b] == false)
			continue;

		int bLeft = (b == 0) ? (binCount - 1) : (b - 1);
		int bRight = (b + 1) % binCount;

		// �������� ����������������� ������� ����������� � ��������
		double peakValue;
		double degreeCorrection;

		InterpolateOrientation (bins[bLeft], bins[b], bins[bRight],
				&degreeCorrection, &peakValue);
			
		//��������� �������������� ��������
		//�������� �� ����� �������� � ��������� [-��, ��]
		double degree = (b + degreeCorrection) * oneBinRad - M_PI;

		if (degree < -M_PI)
			degree += 2.0 * M_PI;
		else if (degree > M_PI)
			degree -= 2.0 * M_PI;
         
		//��������� ���������� �������� ����� � �� ������������
		//��������� � �����������
		CKeypoint *kp = new CKeypoint(point.level,
			point.x + point.local.fineX,
			point.y + point.local.fineY,
			basePixScale, kpSigmaInScale, degree);

		keypoints.push_back(*kp);
		delete kp;
	}
	delete [] binIsKeypoint;
	delete [] bins;

	return keypoints;
};
//---------------------------------------------------------------------------------------
// ������������ � ����� �� ������
void CScaleSpace::ThresholdAndNormalizeFV (CKeypoint *kp, double fvGradThresh)
{
	//������������
	double norm = 0.0;
	for (int n = 0 ; n < kp->FVDim; n++)
		norm += pow(kp->featureVector[n], 2.0);

	norm = sqrt (norm);
	if (norm != 0.0)
	{
		for (int n = 0 ; n < kp->FVDim; n++)
			kp->featureVector[n] = kp->featureVector[n] / norm;
	}

	// ���������� ����� ������������
	for (int n = 0 ; n < kp->FVDim; n++) 
	{
		if (kp->featureVector[n] > fvGradThresh) 
		{
			kp->featureVector[n] = fvGradThresh;
		}
	}

	// ��������������
	norm = 0.0;
	for (int n = 0 ; n < kp->FVDim; n++)
		norm += pow (kp->featureVector[n], 2.0);

	norm = sqrt (norm);
   if (norm != 0.0)
	{
		for (int n = 0 ; n < kp->FVDim ; n++)
			kp->featureVector[n] = kp->featureVector[n] / norm;
	}

};
//---------------------------------------------------------------------------------------    
bool CScaleSpace::InterpolateOrientation (double left, double middle,
		double right, double* degreeCorrection, double* peakValue)
{
	//���������������� ���� �������� �������� ����������� ���������
	//� ����� �������� ������������ ��������.
	//�������� �������� �� ���� ������
	//(-1.0 ; left), (0.0 ; middle) � (1.0 ; right).
	// ������� ��� ��������� a, b,c �������� �� ������� ������� 3� ���������
	// �� �������:
	// f(x) = a (x - c)^2 + b
	// (c, b) - ������� ��������
		
	double a = ((left + right) - 2.0 * middle) / 2.0;
	*degreeCorrection = 0.0;
	*peakValue = 0.0;

	// �� ��������
	if (a == 0.0)
		return false;

	double c = (((left - middle) / a) - 1.0) / 2.0;
	double b = middle - c * c * a;

	if ((c < -0.5) || (c > 0.5))
		return false;
		
	*degreeCorrection = c;
	*peakValue = b;

	return true;
};
//---------------------------------------------------------------------------------------
bool CScaleSpace::IsInCircle (int rX, int rY, int radiusSq)
{
	rX *= rX;
	rY *= rY;
	if ((rX + rY) <= radiusSq)
		return true;
	return false;
};
//---------------------------------------------------------------------------------------
int CScaleSpace::FindClosestBin (int binCount, double angle)
{
   //angle � ��������� [-�, �]
	//����� �������� ������������� �������� �����
	//�������� �� ���� �� ��
	angle += M_PI;
	angle /= 2.0 * M_PI;

	angle *= binCount;

	int idx = (int) angle;
	if (idx == binCount)
		idx = 0;

	return idx;
};
//---------------------------------------------------------------------------------------
CKeypoint::~CKeypoint ()
{
};
//---------------------------------------------------------------------------------------
CKeypoint::CKeypoint ()
{
	this->x = 0;
	this->y = 0;
	this->level = 0;
	this->imgScale = 0;
	this->scale = 0;
	this->orientation = 0;
	this->FVDim = FVDIM;
	
	for (int i=0; i < this->FVDim; i++)
		this->featureVector[i] = 0.0;
	this->oDim = 8;
	this->xDim = 4;
	this->yDim = 4;
	this->hasFV = 0;
};
//---------------------------------------------------------------------------------------
CKeypoint::CKeypoint (int level, double x, double y, double imgScale,
		double kpScale, double orientation)
{
	this->x = x;
	this->y = y;
	this->level = level;
	this->imgScale = imgScale;
	this->scale = kpScale;
	this->orientation = orientation;
	this->FVDim = FVDIM;
	
	for (int i=0; i < this->FVDim; i++)
		this->featureVector[i] = 0.0;
	this->oDim = 8;
	this->xDim = 4;
	this->yDim = 4;
	this->hasFV = 0;
};
//---------------------------------------------------------------------------------------
//���������� �����������
CKeypoint::CKeypoint(const CKeypoint &copy)
{
   this->x = copy.x;
	this->y = copy.y;
	this->level = copy.level;
	this->imgScale = copy.imgScale;
	this->scale = copy.scale;
	this->orientation = copy.orientation;
	this->FVDim = copy.FVDim;
	
	for (int i=0; i < this->FVDim; i++)
		this->featureVector[i] = copy.featureVector[i];
	this->oDim = copy.oDim;
	this->xDim = copy.xDim;
	this->yDim = copy.yDim;
	this->hasFV = copy.hasFV; 
};

//---------------------------------------------------------------------------------------
void CKeypoint::FVSet (int xI, int yI, int oI, double value)
{
	featureVector[(xI * yDim * oDim) + (yI * oDim) + oI] = value;
};
//---------------------------------------------------------------------------------------
double CKeypoint::FVGet (int xI, int yI, int oI)
{
	return (featureVector[(xI * yDim * oDim) + (yI * oDim) + oI]);
};
//---------------------------------------------------------------------------------------
//void CKeypoint::PrintFV (char *filename)
//{
//	FILE *FPFV;
//	FPFV = fopen(filename, "a");
//	for (int i=0; i<FVDIM; i++)
//	fprintf(FPFV, "%.3f\t", featureVector[i]);
//	fputs("\n", FPFV);
//
//	fclose(FPFV);
//};
//---------------------------------------------------------------------------------------
CKeypoint & CKeypoint :: operator =(const CKeypoint &kp)
{
	if (this != &kp)
	{
		this->x = kp.x;
		this->y = kp.y;
		this->level = kp.level;
		this->imgScale = kp.imgScale;
		this->scale = kp.scale;
		this->orientation = kp.orientation;
		this->FVDim = kp.FVDim;
		
		for (int i=0; i < this->FVDim; i++)
			this->featureVector[i] = kp.featureVector[i];
		this->oDim = kp.oDim;
		this->xDim = kp.xDim;
		this->yDim = kp.yDim;
		this->hasFV = kp.hasFV;   
	}
	return *this;
};
//---------------------------------------------------------------------------------------
void CMatchKeypoints::FindMatches (TVKeypoints keys1, TVKeypoints keys2)
{
 	int *compared = new int[int(keys2.size())];
	int countCompared = 0;
	bool IsCompare = false;
	int reserv = int(keys1.size());
	if (int(keys2.size()) > reserv)
		reserv = int(keys2.size());

	matches.reserve(reserv);
	for (int i=0; i<int(keys1.size()); i++)
	{
		double distNearest = 1.0e+10;
		int nearest = -1;
		double dist2Nearest = 1.0e+10;
		int nearest2 = -1;

		for (int k=0; k < int(keys2.size()); k++)
		{
		
			double dist = EuclidDist(keys1[i].featureVector, keys2[k].featureVector);
			if (dist < distNearest) 
			{
				nearest2 = nearest;
				dist2Nearest = distNearest;

				nearest = k;
				distNearest = dist;
			}
		}

		if (nearest == -1 || nearest2 == -1)
			continue;

		if ((distNearest / dist2Nearest) > 0.65)
			continue;
      
		//����������
		//��������� �� ����������� �� ��� ����� ������ � ����������
      for (int c = 0; c < countCompared; c++)
		{
			if (compared[c] == nearest)
			{
				IsCompare = true;
				break;
			}
		}
     
		if (IsCompare)
		{
			IsCompare = false;
			continue;
		}

		compared[countCompared] = nearest;
		countCompared++;
		
		CMatch match(keys1[i], keys2[nearest], distNearest, dist2Nearest);       
		matches.push_back(match);
	}
	delete [] compared;
};
//---------------------------------------------------------------------------------------
double CMatchKeypoints::EuclidDist(double *FV1, double *FV2)
{
	double euDist = 0.0;
	//double distNearest = 1.0e+10;
	for (int i=0; i < FVDIM; i++)
	{
		euDist += (FV1[i] - FV2[i])*(FV1[i] - FV2[i]);
	}
	return sqrt(euDist);
};
//---------------------------------------------------------------------------------------
//void CMatchKeypoints::PrintMatches()
//{
//	FILE *FM;
//	FM = fopen("c:\\matches", "w");
//	if (FM != NULL)
//	{
//		fputs("Image1\t\t\tImage2\n", FM);
//		for (int i=0; i < int(matches.size()); i++)
//		{
//			fprintf(FM, "Match %d\n", i+1);
//			
//			fprintf(FM, "x=%d y=%d\t\tx=%d y=%d\nDist1 = %.3f\n", 
//				(int)matches[i].keys1.x, (int)matches[i].keys1.y, 
//				(int)matches[i].keys2.x, (int)matches[i].keys2.y,
//				matches[i].dist1);
////			matches[i].keys1.PrintFV("e:\\keys1");
////			matches[i].keys2.PrintFV("e:\\keys2");
//		}
//		fclose(FM);
//	}
//};
//---------------------------------------------------------------------------------------
CMatch::CMatch(CKeypoint keys1, CKeypoint keys2, double dist1, double dist2)
{
	this->keys1 = keys1;
	this->keys2 = keys2;
	this->dist1 = dist1;
	this->dist2 = dist2;
};
//---------------------------------------------------------------------------------------
CMatch::CMatch(const CMatch &copy)
{
	this->keys1 = copy.keys1;
	this->keys2 = copy.keys2;
	this->dist1 = copy.dist1;
	this->dist2 = copy.dist2;
};
//---------------------------------------------------------------------------------------