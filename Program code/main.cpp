#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include <fstream>
#include "functions.hpp"





void ZapisDoPliku(const cv::Mat& image, const cv::String& filename, const cv::String& prefix)
{

	cv::String name(prefix);
	name += filename + ".txt";
	std::ofstream oFile(name, std::ios::out);

	if (oFile)
	{
		oFile << "width " + filename << std::endl;
		oFile << "[nr] [pixval]" << std::endl;

		std::vector<int> line;
		line.reserve(image.cols);
		for (int i = 0; i < image.cols; ++i)
		{
			int val = static_cast<int>(image.at<uchar>(cv::Point(i, 100)));
			line.push_back(val);
		}

		int i = 1;
		for (auto& it : line)
		{
			oFile << i << " " << it << std::endl;
			++i;
		}
	}
}



void ProgramProcedure(cv::Mat& image, const cv::String& name)
{
	static unsigned k = 1;

	std::stringstream str;
	str << k;
	cv::String uniq(str.str());

	// Konwersja do HSV:
	cv::Mat hsv;
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

	// Podzial HSV na kanaly:
	std::vector<cv::Mat> hsvc;
	cv::split(hsv, hsvc);

	cv::Mat gray;
	hsvc[2].copyTo(gray);

	// Usuwanie tekstu:
	RemoveText(gray, gray);

	// Wygladzanie:
	//cv::GaussianBlur(gray, gray, cv::Size(5, 5), 4.5);
	cv::GaussianBlur(gray, gray, cv::Size(5, 5), 16.5);

	// Krawedzie:

	cv::Mat edges;
	FindEdges(gray, edges);

	cv::Mat dilero;
	edges.copyTo(dilero);

	cv::dilate(dilero, dilero, cv::Mat());
	cv::dilate(dilero, dilero, cv::Mat());
	cv::dilate(dilero, dilero, cv::Mat());
	cv::erode(dilero, dilero, cv::Mat());


	cv::Mat dst;
	image.copyTo(dst);

	std::vector<cv::Rect> rects = FindRects(dilero);
	MarkEdges(dst, rects);
		

	WindowShow(uniq + name, dst);



	// Krawedzie:

	/*
	const int M = gray.cols;
	const int N = gray.rows;

	// Srednia Ai:

	std::vector<float> Ai;
	Ai.reserve(N);

	for (int i = 0; i < N; ++i)
	{
		int aitemp = 0;

		for (int j = 0; j < M; ++j)
		{
			aitemp += static_cast<int>(gray.at<uchar>(cv::Point(j, i)));
		}

		Ai.push_back(static_cast<float>(aitemp /M));
	}

	// Srednia Aj:

	std::vector<float> Aj;
	Aj.reserve(M);

	for (int j = 0; j < M; ++j)
	{
		int ajtemp = 0;

		for (int i = 0; i < N; ++i)
		{
			ajtemp += static_cast<int>(gray.at<uchar>(cv::Point(j, i)));
		}

		Aj.push_back(static_cast<float>(ajtemp / N));
	}

	// Wariancja Vn:
	*/



	++k;
}



int main(int argc, char* argv[])
{
	// Wczytanie obrazu z pliku:

#ifdef _DEBUG

	const cv::String files[] = { "M1_1.jpg", "M1_3.jpg", "M4_14.jpg", "M4_24.jpg", "H6_d3_s2_0st_40um_1.jpg" };
	//const cv::String files[] = { "M1_3.jpg" };

	for (const auto& file : files)
	{
		cv::Mat image = cv::imread(file, cv::IMREAD_COLOR);

		if (!image.empty())
			ProgramProcedure(image, file);
	}

#else

	for (int i = 1; i < argc; ++i)
	{
		cv::Mat image = cv::imread(argv[i], cv::IMREAD_COLOR);

		if (!image.empty())
			ProgramProcedure(image, argv[i]);
	}

#endif

	cv::waitKey(0);

	return 0;
}