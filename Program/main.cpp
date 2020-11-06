#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp" 



int main(int argc, char* argv[])
{
	// Wczytanie obrazu z pliku:

	cv::Mat image;

	if (argc != 2)
		return -1;
	image = cv::imread(argv[1], cv::IMREAD_COLOR);
	//image = cv::imread("M1_3.jpg", cv::IMREAD_COLOR);

	if (image.empty())
	{
		std::cout << "Couldn't open or find the file!" << std::endl;
		std::cout << "Path: " << argv[1] << std::endl;
		return -1;
	}

	// Zmiana przestrzeni barw na HSV:

	cv::Mat image_hsv;
	cv::cvtColor(image, image_hsv, cv::COLOR_BGR2HSV);

	// Filtracja Gaussa:

	cv::Mat image_gauss;
	cv::GaussianBlur(image_hsv, image_gauss, cv::Size(5, 5), 1.5);

	// Filtracja Canny'ego:

	const int threshold = 16;

	cv::Mat image_canny;
	cv::Canny(image_gauss, image_canny, 0, threshold);

	//Kontury:

	cv::Mat edges;
	cv::cvtColor(image_canny, edges, cv::COLOR_GRAY2BGR);
	
	cv::Mat dest(image);


	for (int j = 0; j < edges.rows; ++j)
	{
		for (int i = 0; i < edges.cols; ++i)
		{
			cv::Vec3b rpixel = edges.at<cv::Vec3b>(cv::Point(i, j));

			unsigned b = static_cast<unsigned>(rpixel[0]);
			unsigned g = static_cast<unsigned>(rpixel[1]);
			unsigned r = static_cast<unsigned>(rpixel[2]);

			if (b == 255 && g == 255 && r == 255)
			{
				cv::Vec3b& wpixel = dest.at<cv::Vec3b>(cv::Point(i, j));
				wpixel[0] = 0;
				wpixel[1] = 255;
				wpixel[2] = 0;
			}
		}
	}

	cv::namedWindow("edges", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("dest", cv::WINDOW_AUTOSIZE);
	cv::imshow("edges", edges);
	cv::imshow("dest", dest);

	cv::waitKey(0);

		
	return 0;
}
