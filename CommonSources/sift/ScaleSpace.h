#pragma once

#define _USE_MATH_DEFINES 
#include <math.h>

#include "Raster.h"
#include "Matrix.h"
#include "gaussconv.h"
#include <vector>

using namespace std;

//все используемые классы

class COctavePyramid;
class CScaleSpace;
class CPointLocalInformation;
class CScalePoint;
class CKeypoint;
class CMatchKeypoints;
class CMatch;

//алиасы
typedef vector<CScaleSpace> TVOctavePyramid;
typedef vector<CScalePoint> TVArrayScalePoints;
typedef vector<CKeypoint> TVKeypoints;
typedef vector<CMatch> TVMatches;

//константы
//игнорируемые значения 
#define DOG_THRESH 0.0075
//максимальное число шагов, которые может сделать простая ключевая точка
//в пространстве, которому она принадлежит
#define RELOCATION_MAXIMUM 4
//отношение главных кривизн
#define EDGE_RATIO 20.0
//Фильтр значений D-функции сохраняет высококонтрастные точки
#define DVALUE_LOW_THRESH 0.008
//отклонение от исходной плоскости
//этим значением отклоняются пики, которые локализованы 
//слишком далеко от исходной плоскости
#define SCALE_ADJUST_THRESH 0.5
//
#define OCTAVE_SIGMA 1.6
//
#define COUNT_SCALES 3
//
#define MIN_SIZE 32
//порог для создания новой ключевой точки
//процентное отношение относительно максимального значение пика
#define KEYPOINT_THRESH 0.8
//размерность характеристического вектора
#define FVDIM 128
//-------------------------------------------------------------------------------------
class CScaleSpace
{
public:
	CScaleSpace(void);
	CScaleSpace(const CScaleSpace &copying);
	~CScaleSpace(void);

	double basePixScale;//базовый масштаб

	//CRaster *imgScaled;//массив размытых изображений
	//int LengthImgScaled;//счетчик изображений

	//массив нормализованных значений
	double ***imgScaledD;
	double *BitsImgScaledD;
	//размерности
	int XimgScaled, YimgScaled, CimgScaled;
    
	//пространства DoG
	//массив нормализованных значений
	double ***spacesD;
	double *BitsSpacesD;

	//размерности
	int Xspaces, Yspaces, Cspaces;
	void ClearSpacesD();
	void ClearImgScaledD();

	//магнитуды и направления
	double ***magnitudes;
	double *BitsMagnitudes;
	double ***directions;
	double *BitsDirections;
	//их количества
	int Cmagdir, Xmagdir, Ymagdir;
	void AllocMagnitudes();
	void AllocDirections();
	void ClearMagnitudes();
	void ClearDirections();
   
	//коэффициент k = 2^(1/s)
	double SToK (int s)	{
		return pow (2.0, 1.0 / s); }
  
	//строит множества Difference-of-Gaussian (DoG) изображений из размытых гауссиановых изображений
	void BuildDiffMaps ();
    //строит s+3 размытых гауссиановых изображений
	//с интервалом k*сигма
    void BuildGaussianMaps(CRaster *first, double firstScale, 
									int scales, double octaveSigma);
	//возвращает последний созданный гауссиан
	//CRaster *LastGaussianMap(){
	//	if (LengthImgScaled < 2) return NULL;
	// return &imgScaled[LengthImgScaled-2];};
    
	//массив пиковых значений
	 TVArrayScalePoints peaks;

	 //поиск пиковых значений
    void FindPeaks ();
    //поиск пиковых значений на трёх уровнях
	void FindPeaksThreeLevel (double** below, double** current,
		double** above, int curLev);
    //проверка является ли пиксель макс или мин на своем уровне	
	void CheckMinMax (double **layer, double c, int x, int y,
		 bool &IsMin, bool &IsMax, bool cLayer);
    
	//массив фильтрованных пиков
    TVArrayScalePoints filtered;
	//фильтрация неустойчивых точек
    void FilterAndLocalizePeaks ();
	//проверка точки по отношению главной кривизны
	//устраняет ключевые точки, у которых отношение между главными кривизнами больше чем 10
	bool CheckEdgeResponses (double** space, int x, int y);
	//проверяет точки по пороговому значению контраста
	//проверяет ослабленный контраст
	bool LocalizeHasLowContrast (CScalePoint *point, int steps);
	
	//возвращает значение экстремума которое проверяется по пороговому значению 
	void GetAdjustment (CScalePoint point, double* dp, Matrix *b);
	//Вычисление значений и направлений градиентов для всех плоскостей
	//одной октавы (ScaleSpace)
	void CreateMagnitudesAndDirections();
	//генерирует ключевые точки
	TVKeypoints GenerateKeypoints();
	//
	TVKeypoints GenerateKeypointSingle(CScalePoint point);
    bool IsInCircle (int rX, int rY, int radiusSq);
	int FindClosestBin (int binCount, double angle);
	//интерполяция параболой трех столбцов гистограммы
	bool InterpolateOrientation (double left, double middle,
		double right, double* degreeCorrection, double* peakValue);
    
	//массив ключевых точек
	//TVKeypoints keypoints;
	TVKeypoints CreateDescriptors(TVKeypoints keypoints, int level);
	
	void ThresholdAndNormalizeFV (CKeypoint *kp, double fvGradHicap);

};
//-------------------------------------------------------------------------------------
//класс, строящий и хранящий все построенные октавы 
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
//класс локальной информации о точке
class CPointLocalInformation
{
public:
	CPointLocalInformation(void){};
	CPointLocalInformation(double fineS_in, double fineX_in, double fineY_in){ fineX = fineX_in; fineY = fineY_in; scaleAdjust = fineS_in;};
	~CPointLocalInformation(void){};
    
	//смещение относительно точки
	//в диапазоне [-0.5 ; 0.5]
	double fineX, fineY;
    //относительный масштаб корректировки к масштабу базового изображения
	double scaleAdjust;
    //значение экстремума    
	double dValue;
  
};
//-------------------------------------------------------------------------------------
//класс точки на пространстве
//хранит координаты и уровень пространства, к которому она принадлежит
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
// Простая ключевая точка, конечный результат создания ключевой точки
// Содержит описатель ключевой точки и ее позицию
class CKeypoint
{
public:
	//уровень изображения, в котором обнаружена точка
	int level;
	//позиция точки на изображении
	double x, y;
    //масштаб изображения, на котором найдена ключевая точка
	double imgScale;
	// абсолютный масштаб ключевой точки, где 1.0 соответствует оригинальному входному изображению
	double scale;
	double orientation;

	// Описатель клюевой точки
	int hasFV;
	//характеристический вектор
	double featureVector[FVDIM];
	//размерность вектора
	int FVDim;

	double FVGet (int xI, int yI, int oI);
	void FVSet (int xI, int yI, int oI, double value);

	int xDim, yDim, oDim;

	~CKeypoint();
	CKeypoint (int level, double x, double y, double imgScale,
		double kpScale, double orientation);
	//копирующий конструктор
	CKeypoint(const CKeypoint &copy);
	CKeypoint();
    //оператор присваивания
//    CKeypoint & operator = (CKeypoint &);
	CKeypoint & operator = (const CKeypoint &);
	//печать ХВ
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
  //копирующий конструктор
  CMatch(const CMatch &copy);
  
};

