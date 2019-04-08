#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;


/*cv::Mat FindIRLEDSpot(

	cv::Mat new_frame, int width, int height, bool *has_led_spot, int *led_spot_x, int *led_spot_y, cv::Mat template_)*/

cv::Mat FindIRLEDSpot(

		cv::Mat new_frame/*const uint8_t *new_frame*/, int width, int height,

		bool *has_led_spot, int *led_spot_x, int *led_spot_y, cv::Mat template_, int *led_spot_size, double *led_spot_confidence)
{
	
	// convert frame to an OpenCV Mat.

	//cv::Mat img(height, width, CV_8U, (void *)new_frame);

	cv::Mat template_match_result;

	cv::Mat threshold_result;

	cv::Mat threshold_result_cv_8U;

	cv::Mat labels;

	cv::Mat stats;

	cv::Mat centroids;

	int ret;


	// Do normalized cross correlation.
	//cv::matchTemplate(img, template_, template_match_result, cv::TM_CCOEFF_NORMED);
	cv::matchTemplate(new_frame, template_, template_match_result, cv::TM_CCOEFF_NORMED);


	// Threshold cross correlation result.

	cv::threshold(template_match_result, threshold_result,

		/*thresh=*/0.7843, /*maxval=*/1.0, cv::THRESH_BINARY);

	threshold_result.convertTo(threshold_result_cv_8U, CV_8U);

	// Find the connected components.

	ret = cv::connectedComponentsWithStats(threshold_result_cv_8U, labels,

		stats, centroids, /*connectivity=*/8, CV_16U);


	// Grab the second largest connected component. The largest component is the background.

	int largest_label = -1;

	int largest_size = -1;

	int second_largest_label = -1;

	int second_largest_size = -1;

	for (int i = 0; i < stats.rows; ++i) {

		int new_size = stats.at<int>(i, cv::CC_STAT_AREA);

		if (new_size > largest_size) {

			second_largest_label = largest_label;

			second_largest_size = largest_size;

			largest_label = i;

			largest_size = new_size;

		}
		else if (new_size > second_largest_label) {

			second_largest_label = i;

			second_largest_size = new_size;

		}

	}

	//VLOG(2) << "second_largest_size: " << second_largest_size;
	std::cout << "second_largest_size: " << second_largest_size <<endl;

	//VLOG(2) << "second_largest_label: " << second_largest_label;
	std::cout << "second_largest_label: " << second_largest_label << endl;

	//VLOG(2) << "stats.rows: " << stats.rows;
	std::cout << "stats.rows: " << stats.rows << endl;


	// Determine if LED spot exists

	static const double kPerfectSpotSize = 210.0;
	*has_led_spot = (second_largest_size > 100);
	//has_led_spot = second_largest_size > 100;

	if (*has_led_spot) {
	//if (has_led_spot) {
		// Calculate the center of the bounding rectangle of the detected spot.

		// The image was cropped after the 'matchTemplate' routine, so we need to

		// add on half the width and height of the template image as well.

		int center_x = stats.at<int>(second_largest_label, cv::CC_STAT_LEFT) +

			stats.at<int>(second_largest_label, cv::CC_STAT_WIDTH) / 2 +

			template_.cols / 2;

		int center_y =

			stats.at<int>(second_largest_label, cv::CC_STAT_TOP) +

			stats.at<int>(second_largest_label, cv::CC_STAT_HEIGHT) / 2 +

			template_.rows / 2;

		//analysis_results.set_led_spot_center_x(center_x);
		*led_spot_x = center_x;

		//analysis_results.set_led_spot_center_y(center_y);
		*led_spot_y = center_y;

		*led_spot_size = second_largest_size;

		// calculate the confidence off of the size of the detected spot. Saturate

		// it at 1.0.

		// TODO(nshemon) Calculate a confidence when no spot is detected.

		double confidence = cv::min(second_largest_size / kPerfectSpotSize, 1.0);

		//analysis_results.set_led_spot_confidence(confidence);
		*led_spot_confidence = confidence;
	}
	return template_match_result;
}


int main(int argc, char** argv)
{
	//read template image
	Mat template_ = imread("../data/ir_template_80x60_gray.png", CV_LOAD_IMAGE_GRAYSCALE);
	if (!template_.data) {
		std::cout << "cv::imread() failed..." << std::endl;
		return -1;
	}

  for (int i = 0; i < 23; i++) { //total 23 images

		string img_file = "../data/SLOW_CAPTURE00";
		string new_file = "../data/SLOW_CAPTURE00";

		 // Read the image file
		int index = 3 * i;
		if (index < 10){
			img_file = img_file + to_string(0) + to_string(index) + "_IMG.bmp";
			new_file = new_file + to_string(0) + to_string(index) + "_IMG_NEW.bmp";
		}
		else {
			img_file = img_file + to_string(index) + "_IMG.bmp";
			new_file = new_file + to_string(index) + "_IMG_NEW.bmp";
		}


		//Mat image = imread("../data/SLOW_CAPTURE0054_IMG.bmp", CV_LOAD_IMAGE_GRAYSCALE);
		Mat image = imread(img_file, CV_LOAD_IMAGE_GRAYSCALE);

		// Check for failure
		if (image.empty())
		{
			cout << "Could not open or find the image" << endl;
			return -1;
		}

		int width = image.cols;
		int height = image.rows;

		//init
		bool has_led_spot = false;
		int led_spot_x = 0;
		int led_spot_y = 0;
		int led_spot_size = 0;
		double led_spot_confidence = 0.0;

		FindIRLEDSpot(image, width, height,

			&has_led_spot, &led_spot_x, &led_spot_y, template_, &led_spot_size, &led_spot_confidence);

		cout << "Find IR LED Spot Result:" << endl;
		cout << "has_led_spot: " << has_led_spot << endl;
		cout << "led_spot_x: " << led_spot_x << endl;
		cout << "led_spot_y: " << led_spot_y << endl;
		cout << "led_spot_size: " << led_spot_size << endl;
		cout << "led_spot_confidence: " << led_spot_confidence << endl;

		cv::Point pt(led_spot_x, led_spot_y); // pixel coordinates


		namedWindow("led spot detected image", WINDOW_AUTOSIZE);// Create a window for display.

		int vertical_length = led_spot_size / 4;
		int horizontal_length = led_spot_size / 4;

		Rect RectangleToDraw4(led_spot_x - horizontal_length + horizontal_length / 2, led_spot_y - vertical_length + vertical_length / 2, led_spot_size / 4, led_spot_size / 4);
		rectangle(image, RectangleToDraw4, Scalar(255, 0, 0), 2, 8, 0);
		imshow("led spot detected image", image);

		imwrite(new_file, image);
		waitKey(2);

		//free memory
		image.release();
	}
	return 0;
}
