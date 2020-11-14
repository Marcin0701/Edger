#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp" 

#include "Window.hpp"



cv::Mat MarkEdges(const cv::Mat& src, const cv::Mat& canny, const std::string& winname)
{
	cv::Mat edges;
	cv::cvtColor(canny, edges, cv::COLOR_GRAY2BGR);

	cv::Mat ret;
	ret.create(src.size(), src.type());
	src.copyTo(ret);

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
				cv::Vec3b& wpixel = ret.at<cv::Vec3b>(cv::Point(i, j));
				wpixel[0] = 0;
				wpixel[1] = 255;
				wpixel[2] = 0;
			}
		}
	}

	Window win(winname, ret);
	win.Show();

	return ret.clone();
}



void ProgramProcedure(cv::Mat& image)
{
	static unsigned i = 1;

	std::stringstream str;
	str << i;

	// BGR -> HSV:
	cv::Mat hsv;
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

	// Podzial HSV na kanaly:
	std::vector<cv::Mat> hsvc;
	cv::split(hsv, hsvc);

	// Wyostrzanie:
	cv::Mat gauss, sharp;
	cv::GaussianBlur(hsvc[2], gauss, cv::Size(0, 0), 25);
	cv::addWeighted(hsvc[2], 1.5, gauss, -0.5, 0, sharp);

	// Wygladzanie:
	cv::Mat smooth;
	cv::GaussianBlur(sharp, smooth, cv::Size(3, 3), 75);

	// Operacja otwarcia (erozja -> dylatacja):
	const cv::Mat elem = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9), cv::Point(-1, -1));
	cv::erode(smooth, smooth, elem);
	cv::dilate(smooth, smooth, elem);

	// Powtorne wygladzanie:
	cv::GaussianBlur(smooth, smooth, cv::Size(3, 3), 75);

	// Znajdowanie linii (algorytm Canny'ego):
	const int threshold = 16;
	cv::Mat canny;
	cv::Canny(smooth, canny, 0, threshold);

	// Wyjscie:
	Window windowc(str.str() + ". Canny", canny);
	windowc.Show();
	MarkEdges(image, canny, str.str() + ". Edges");

	++i;
}



int main(int argc, char* argv[])
{
	// Wczytanie obrazu z pliku:

#ifdef _DEBUG

	const cv::String files[] = { "M1_1.jpg", "M1_3.jpg", "H6_d3_s2_0st_40um_1.jpg", "M4_14.jpg" };

	for (const auto& file : files)
	{
		cv::Mat image = cv::imread(file, cv::IMREAD_COLOR);

		if (!image.empty())
			ProgramProcedure(image);
	}

#else
	
	for (int i = 1; i < argc; ++i)
	{
		cv::Mat image = cv::imread(argv[i], cv::IMREAD_COLOR);

		if (!image.empty())
			ProgramProcedure(image);
	}

#endif

	cv::waitKey(0);

	return 0;
}