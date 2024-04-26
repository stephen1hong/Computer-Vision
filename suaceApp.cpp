//github ravimalb/sauce
//Example usage of SUACE contrast enhancement algorithm for retinal images

#include <opencv2\opencv.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "suace.h"
using namespace cv;
using namespace std;

int a = 50;     //control surounding background/noise level, the larger the smoother; the less the noiser[40-60]
int b = 200;    //control sigma of gaussian filter; too larger (e.g, 400), whiting the image  [200-300]
int intensityMax = 255;

int main()
{
	namedWindow("SUACE", 1);
	createTrackbar("distance", "SUACE", &a, intensityMax);
	createTrackbar("sigma", "SUACE", &b, intensityMax);
	char filename[100];
	Mat suaceResult1, suaceResult2;
	//Mat frame

	while (true)
	{
		Mat image01 = imread("C:\\Hong\\8.png", 1);// slave image
		Mat image02 = imread("C:\\Hong\\7.png", 1);  //reference image, master

		if (!image01.data || !image02.data)
		{
			std::cout << " --(!) Error reading images " << std::endl; return -1;
		}

		Mat image1, image2;

		cvtColor(image01, image1, CV_BGR2GRAY); //to be processed image
		cvtColor(image02, image2, CV_BGR2GRAY);//reference image
		namedWindow("image1", WINDOW_NORMAL);
		imshow("image1", image1);
		namedWindow("image2", WINDOW_NORMAL);
		imshow("image2", image2);
		waitKey(1);

		//frame = imread("C:\\path\\to\\your\\image.png", CV_LOAD_IMAGE_GRAYSCALE);
		//frame=imread("C:\\Hong\\master_img.png", CV_LOAD_IMAGE_GRAYSCALE);
		//performSUACE(frame, suaceResult, a, (b + 1) / 8.0); //perform SUACE with the parameters
		performSUACE(image1, suaceResult1, a, (b + 1) / 8.0); 
		performSUACE(image2, suaceResult2, a, (b + 1) / 8.0);
		namedWindow("SAUCE1", WINDOW_NORMAL);
		imshow("SAUCE1", suaceResult1);
		namedWindow("SAUCE2", WINDOW_NORMAL);
		imshow("SAUCE2", suaceResult2);

		waitKey();

		string outFilename1 = "C:\\Hong\\imageresults\\_sauce8_a50b200";
		imwrite(outFilename1 + ".png", suaceResult1);
		string outFilename2 = "C:\\Hong\\imageresults\\_sauce7_a50b200";
		imwrite(outFilename2 + ".png", suaceResult2);

		int response = waitKey(0);//press key to update
		if (response == 32) //exit when spacebar key pressed;
			break;
	}
	return 0;
}
