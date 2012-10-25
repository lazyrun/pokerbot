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
	//���������
	//img->ScaleDouble();

	//��������������� ����������
	double **res;
	double *Bits;
	res = new double*[img->Width()];
	Bits = new double[img->Width()*img->Height()];
	
	for (int i=0; i < img->Width(); i++)
		res[i] = Bits + i*img->Height();

	CGaussConv *gauss = new CGaussConv (1.5);
	gauss->Convolve(img->PtrValues(), res, img->Width(), img->Height());

	//����������� ���������
	for (int y=0; y < img->Height(); y++)
	{
		for (int x=0; x < img->Width(); x++)
		{
			img->Set(x, y, res[x][y]);
		}
	}
	//��������
	delete [] res;
	delete [] Bits;
	 
	//�������� �����
	COctavePyramid pyr;
	
	pyr.BuildOctaves(img, COUNT_SCALES, OCTAVE_SIGMA, MIN_SIZE);
	 
	//���� ����

	for (int i=0; i < ((int)pyr.octaves.size()); i++)
	{
		//����� ������� �������� ��� ������ ������ � ������ ��������� (DoG) � ������
		pyr.octaves[i].FindPeaks();
		//���� ��������������� �����
		//int oldCount = (int) pyr.octaves[i].peaks.size();
		//������������ ����
		pyr.octaves[i].FilterAndLocalizePeaks();
		//����� ���������� �����
		//int newCount = (int) pyr.octaves[i].filtered.size();
		//��������� ��������� � ���������� ��� ������ ���������
		pyr.octaves[i].CreateMagnitudesAndDirections();
		//������� ����������� �������� �����
	 
		TVKeypoints kp1 = pyr.octaves[i].GenerateKeypoints();
		keypoints.insert(keypoints.end(), kp1.begin(), kp1.end());

		pyr.octaves[i].ClearDirections();
		pyr.octaves[i].ClearMagnitudes();

	}

}
