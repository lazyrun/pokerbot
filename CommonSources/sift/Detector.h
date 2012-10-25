#pragma once
#include "Raster.h"
#include "ScaleSpace.h"

class CDetector
{
public:
	CDetector(void);
	~CDetector(void);

	CDetector(CRaster *img);
	TVKeypoints keypoints;
	
};
