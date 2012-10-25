#include "detector.h"

//---------------------------------------------------------------------------------------
CDetector::CDetector(void)
{
}
//---------------------------------------------------------------------------------------
CDetector::~CDetector(void)
{
}
//---------------------------------------------------------------------------------------
CDetector::CDetector(CRaster *img)
{
	//увеличить
	//img->ScaleDouble();

	//предварительное размывание
	double **res;
	double *Bits;
	res = new double*[img->Width()];
	Bits = new double[img->Width()*img->Height()];
	
	for (int i=0; i < img->Width(); i++)
		res[i] = Bits + i*img->Height();

	CGaussConv *gauss = new CGaussConv (1.5);
	gauss->Convolve(img->PtrValues(), res, img->Width(), img->Height());

	//скопировать результат
	for (int y=0; y < img->Height(); y++)
	{
		for (int x=0; x < img->Width(); x++)
		{
			img->Set(x, y, res[x][y]);
		}
	}
	//очистить
	delete [] res;
	delete [] Bits;
	 
	//пирамида октав
	COctavePyramid pyr;
	
	pyr.BuildOctaves(img, COUNT_SCALES, OCTAVE_SIGMA, MIN_SIZE);
	 
	//ищем пики

	for (int i=0; i < ((int)pyr.octaves.size()); i++)
	{
		//поиск пиковых значений для каждой октавы и каждой плоскости (DoG) в октаве
		pyr.octaves[i].FindPeaks();
		//было нефильтрованных пиков
		//int oldCount = (int) pyr.octaves[i].peaks.size();
		//профильтруем пики
		pyr.octaves[i].FilterAndLocalizePeaks();
		//новое количество пиков
		//int newCount = (int) pyr.octaves[i].filtered.size();
		//создаются магнитуды и направлния для каждой плоскости
		pyr.octaves[i].CreateMagnitudesAndDirections();
		//создаем дескрипторы ключевых точек
	 
		TVKeypoints kp1 = pyr.octaves[i].GenerateKeypoints();
		keypoints.insert(keypoints.end(), kp1.begin(), kp1.end());

		pyr.octaves[i].ClearDirections();
		pyr.octaves[i].ClearMagnitudes();

	}

}
