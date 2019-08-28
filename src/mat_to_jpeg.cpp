#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "include/mat_to_jpeg.h"
JPEG::JPEG()
{

}

JPEG::~JPEG()
{

}

void JPEG::JPEG_enCode(Mat &src,std::vector<unsigned char>&buff)
{
	vector<int> param = vector<int>(2);
	param[0]=CV_IMWRITE_JPEG_QUALITY;
	param[1]=95;//default(95) 0-100
			 
	imencode(".jpg",src,buff,param);
}

void JPEG::JPEG_deCode(Mat &src,vector<unsigned char>&buff)
{
	src = imdecode(Mat(buff),CV_LOAD_IMAGE_COLOR);
}
