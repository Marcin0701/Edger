#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp" 

#include <vector>
#include "Window.hpp"


cv::Mat ToHSV(const cv::Mat& img)
{
	cv::Mat ret;
	cv::cvtColor(img, ret, cv::COLOR_BGR2HSV);
	Window win("HSV");
	win.Show(ret);
	return ret.clone();
}

std::vector<cv::Mat> ToHSVChannels(const cv::Mat& img)
{
	std::vector<cv::Mat> ret;
	cv::split(img, ret);
	return ret;
}

cv::Mat GeHSVChannel(const std::vector<cv::Mat>& imgs, const unsigned nr)
{
	return imgs[nr];
}

cv::Mat ToCanny(const cv::Mat& img, const int threshold = 16)
{
	cv::Mat ret;
	cv::Canny(img, ret, 0, threshold);
	Window win("Canny");
	win.Show(ret);
	return ret.clone();
}

cv::Mat ToEdges(const cv::Mat& src, const cv::Mat& canny)
{
	cv::Mat edges;
	cv::cvtColor(canny, edges, cv::COLOR_GRAY2BGR);

	Window win("Edges");
	win.Show(edges);

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

	return ret.clone();
}

cv::Mat ToGauss(const cv::Mat& img)
{
	cv::Mat ret;
	cv::GaussianBlur(img, ret, cv::Size(5, 5), 1.5);
	Window win("Gauss");
	win.Show(ret);
	return ret.clone();
}


int main(int argc, char* argv[])
{
	// Wczytanie obrazu z pliku:

	cv::Mat src;
	const cv::String path[] = { "M1_1.jpg", "M1_3.jpg", "H6_d3_s2_0st_40um_1.jpg" };

	src = cv::imread(path[2], cv::IMREAD_COLOR);

	if (src.empty())
	{
		std::cout << "Couldn't open or find the file!" << std::endl;
		return -1;
	}

	auto hsv = ToHSV(src);
	auto hsvch = ToHSVChannels(hsv);
	auto val = GeHSVChannel(hsvch, 2);
	auto gauss = ToGauss(val);
	auto canny = ToCanny(gauss);
	auto dst = ToEdges(src, canny);

	Window win("Src");
	Window win2("Dst");
	win.Show(src);
	win2.Show(dst);

	cv::waitKey(0);

	return 0;
}