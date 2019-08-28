#ifndef _MATTOJPEG_H
#define _MATTOJPEG_H

#include <vector>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;
class JPEG
{
	public:
		JPEG();
		~JPEG();
		void JPEG_enCode(Mat &src,vector<unsigned char>&buff);
		void JPEG_deCode(Mat &src,vector<unsigned char>&buff);
};


#endif
