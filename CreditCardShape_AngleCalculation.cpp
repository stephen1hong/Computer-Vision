
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/stitching/detail/blenders.hpp"  
#include <string>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;


int main(int argc, char **argv)
{

//	string fileName;
	ofstream anglefile;
	anglefile.open("angles.txt");

	for (int i = 1; i < 40; i++) {
		string fileName = "../data/mask/" + to_string(i) + ".png";
		cout << "fileName: " << fileName << endl;
		Mat image1 = imread(fileName);

		if (!image1.data)
		{
			std::cout << " --(!) Error reading images " << std::endl; return -1;
		}

		Mat gray1,  mask1;
		cvtColor(image1, gray1, CV_BGR2GRAY);  //to be processed image


		int threshold = 200;
		mask1 = gray1 > threshold;
		//mask2 = gray2 > threshold;

		// extract contours
		vector<std::vector<cv::Point> > contours;
		cv::findContours(mask1, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	
		for (int i = 0; i < contours.size(); ++i)
		{
			// fit bounding rectangle around contour
			cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);

			// read points and angle
			cv::Point2f rect_points[4];
			rotatedRect.points(rect_points);

		//choose the longer edge of the rotated rect to computer angle
		Point2f edge1 = Vec2f(rect_points[1].x, rect_points[1].y) - Vec2f(rect_points[0].x, rect_points[0].y);
		Point2f edge2 = Vec2f(rect_points[2].x, rect_points[2].y) - Vec2f(rect_points[1].x, rect_points[1].y);

		Point2f longerEdge = edge1;
		if (norm(longerEdge) < norm(edge2))
			longerEdge = edge2;

			//horizontal edge
			Point2f horizontal = Vec2f(1, 0);

			float angle = 180.0f / CV_PI *acos((horizontal.x *longerEdge.x + horizontal.y * longerEdge.y) / (norm(horizontal) * norm(longerEdge)));


			/*// read center of rotated rect
			Point2f center = rotatedRect.center; // center

			// draw rotated rect
			for (unsigned int j = 0; j < 4; ++j)
				line(image1, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0));

			// draw center and print text
			stringstream ss;   ss << angle; // convert float to string
			circle(image1, center, 5, Scalar(0, 255, 0)); // draw center
			putText(image1, ss.str(), center + Point2f(-25, 25), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255, 0, 255)); // print angle*/

			cout << "angle: " << angle << endl;
			anglefile << fileName << "," << "angle=" << angle << endl;

		}
	}
	anglefile.close();
	return 0;
}
