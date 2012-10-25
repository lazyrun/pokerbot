#pragma once

#define _USE_MATH_DEFINES 
#include <math.h>

#include "Raster.h"
#include "Matrix.h"
#include "gaussconv.h"
#include <vector>

using namespace std;

//��� ������������ ������

class COctavePyramid;
class CScaleSpace;
class CPointLocalInformation;
class CScalePoint;
class CKeypoint;
class CMatchKeypoints;
class CMatch;

//������
typedef vector<CScaleSpace> TVOctavePyramid;
typedef vector<CScalePoint> TVArrayScalePoints;
typedef vector<CKeypoint> TVKeypoints;
typedef vector<CMatch> TVMatches;

//���������
//������������ �������� 
#define DOG_THRESH 0.0075
//������������ ����� �����, ������� ����� ������� ������� �������� �����
//� ������������, �������� ��� �����������
#define RELOCATION_MAXIMUM 4
//��������� ������� �������
#define EDGE_RATIO 20.0
//������ �������� D-������� ��������� ����������������� �����
#define DVALUE_LOW_THRESH 0.008
//���������� �� �������� ���������
//���� ��������� ����������� ����, ������� ������������ 
//������� ������ �� �������� ���������
#define SCALE_ADJUST_THRESH 0.5
//
#define OCTAVE_SIGMA 1.6
//
#define COUNT_SCALES 3
//
#define MIN_SIZE 32
//����� ��� �������� ����� �������� �����
//���������� ��������� ������������ ������������� �������� ����
#define KEYPOINT_THRESH 0.8
//����������� ������������������� �������
#define FVDIM 128
//-------------------------------------------------------------------------------------
class CScaleSpace
{
public:
	CScaleSpace(void);
	CScaleSpace(const CScaleSpace &copying);
	~CScaleSpace(void);

	double basePixScale;//������� �������

	//CRaster *imgScaled;//������ �������� �����������
	//int LengthImgScaled;//������� �����������

	//������ ��������������� ��������
	double ***imgScaledD;
	double *BitsImgScaledD;
	//�����������
	int XimgScaled, YimgScaled, CimgScaled;
    
	//������������ DoG
	//������ ��������������� ��������
	double ***spacesD;
	double *BitsSpacesD;

	//�����������
	int Xspaces, Yspaces, Cspaces;
	void ClearSpacesD();
	void ClearImgScaledD();

	//��������� � �����������
	double ***magnitudes;
	double *BitsMagnitudes;
	double ***directions;
	double *BitsDirections;
	//�� ����������
	int Cmagdir, Xmagdir, Ymagdir;
	void AllocMagnitudes();
	void AllocDirections();
	void ClearMagnitudes();
	void ClearDirections();
   
	//����������� k = 2^(1/s)
	double SToK (int s)	{
		return pow (2.0, 1.0 / s); }
  
	//������ ��������� Difference-of-Gaussian (DoG) ����������� �� �������� ������������ �����������
	void BuildDiffMaps ();
    //������ s+3 �������� ������������ �����������
	//� ���������� k*�����
    void BuildGaussianMaps(CRaster *first, double firstScale, 
									int scales, double octaveSigma);
	//���������� ��������� ��������� ��������
	//CRaster *LastGaussianMap(){
	//	if (LengthImgScaled < 2) return NULL;
	// return &imgScaled[LengthImgScaled-2];};
    
	//������ ������� ��������
	 TVArrayScalePoints peaks;

	 //����� ������� ��������
    void FindPeaks ();
    //����� ������� �������� �� ��� �������
	void FindPeaksThreeLevel (double** below, double** current,
		double** above, int curLev);
    //�������� �������� �� ������� ���� ��� ��� �� ����� ������	
	void CheckMinMax (double **layer, double c, int x, int y,
		 bool &IsMin, bool &IsMax, bool cLayer);
    
	//������ ������������� �����
    TVArrayScalePoints filtered;
	//���������� ������������ �����
    void FilterAndLocalizePeaks ();
	//�������� ����� �� ��������� ������� ��������
	//��������� �������� �����, � ������� ��������� ����� �������� ���������� ������ ��� 10
	bool CheckEdgeResponses (double** space, int x, int y);
	//��������� ����� �� ���������� �������� ���������
	//��������� ����������� ��������
	bool LocalizeHasLowContrast (CScalePoint *point, int steps);
	
	//���������� �������� ���������� ������� ����������� �� ���������� �������� 
	void GetAdjustment (CScalePoint point, double* dp, Matrix *b);
	//���������� �������� � ����������� ���������� ��� ���� ����������
	//����� ������ (ScaleSpace)
	void CreateMagnitudesAndDirections();
	//���������� �������� �����
	TVKeypoints GenerateKeypoints();
	//
	TVKeypoints GenerateKeypointSingle(CScalePoint point);
    bool IsInCircle (int rX, int rY, int radiusSq);
	int FindClosestBin (int binCount, double angle);
	//������������ ��������� ���� �������� �����������
	bool InterpolateOrientation (double left, double middle,
		double right, double* degreeCorrection, double* peakValue);
    
	//������ �������� �����
	//TVKeypoints keypoints;
	TVKeypoints CreateDescriptors(TVKeypoints keypoints, int level);
	
	void ThresholdAndNormalizeFV (CKeypoint *kp, double fvGradHicap);

};
//-------------------------------------------------------------------------------------
//�����, �������� � �������� ��� ����������� ������ 
class COctavePyramid
{
public:
  COctavePyramid();
  ~COctavePyramid();
  
 TVOctavePyramid octaves;
 
 void BuildOctaves (CRaster *source,
		int levelsPerOctave, double octaveSigma, int minSize);
};
//-------------------------------------------------------------------------------------
//����� ��������� ���������� � �����
class CPointLocalInformation
{
public:
	CPointLocalInformation(void){};
	CPointLocalInformation(double fineS_in, double fineX_in, double fineY_in){ fineX = fineX_in; fineY = fineY_in; scaleAdjust = fineS_in;};
	~CPointLocalInformation(void){};
    
	//�������� ������������ �����
	//� ��������� [-0.5 ; 0.5]
	double fineX, fineY;
    //������������� ������� ������������� � �������� �������� �����������
	double scaleAdjust;
    //�������� ����������    
	double dValue;
  
};
//-------------------------------------------------------------------------------------
//����� ����� �� ������������
//������ ���������� � ������� ������������, � �������� ��� �����������
class CScalePoint
{
public:
    CPointLocalInformation local;
	int x, y;
	int level;

	CScalePoint(int X, int Y, int Level){x = X; y = Y; level = Level;};
	~CScalePoint(){};
};
//-------------------------------------------------------------------------------------
// ������� �������� �����, �������� ��������� �������� �������� �����
// �������� ��������� �������� ����� � �� �������
class CKeypoint
{
public:
	//������� �����������, � ������� ���������� �����
	int level;
	//������� ����� �� �����������
	double x, y;
    //������� �����������, �� ������� ������� �������� �����
	double imgScale;
	// ���������� ������� �������� �����, ��� 1.0 ������������� ������������� �������� �����������
	double scale;
	double orientation;

	// ��������� ������� �����
	int hasFV;
	//������������������ ������
	double featureVector[FVDIM];
	//����������� �������
	int FVDim;

	double FVGet (int xI, int yI, int oI);
	void FVSet (int xI, int yI, int oI, double value);

	int xDim, yDim, oDim;

	~CKeypoint();
	CKeypoint (int level, double x, double y, double imgScale,
		double kpScale, double orientation);
	//���������� �����������
	CKeypoint(const CKeypoint &copy);
	CKeypoint();
    //�������� ������������
//    CKeypoint & operator = (CKeypoint &);
	CKeypoint & operator = (const CKeypoint &);
	//������ ��
	//void PrintFV (char *filename);
};
//-------------------------------------------------------------------------------------
class CMatchKeypoints
{
  TVKeypoints img1, img2;
public:
	TVMatches matches;
	CMatchKeypoints(){};
	~CMatchKeypoints(){};
	
	double EuclidDist(double *FV1, double *FV2);
	void FindMatches (TVKeypoints keys1, TVKeypoints keys2);
	//void PrintMatches ();
	
};
//-------------------------------------------------------------------------------------
class CMatch
{
public:
  CKeypoint keys1, keys2;
  double dist1, dist2;

  CMatch(CKeypoint keys1, CKeypoint keys2, double dist1, double dist2);
  CMatch(){};
  //���������� �����������
  CMatch(const CMatch &copy);
  
};

