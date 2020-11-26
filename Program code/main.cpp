#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp" 


#include <fstream>


void ShowWindow(const cv::String& name, const cv::Mat& img)
{
	cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
	cv::imshow(name, img);
}


cv::Mat MarkEdges(const cv::Mat& src, const cv::Mat& canny, const cv::String& winname)
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


	ShowWindow(winname, ret);

	return ret.clone();
}


cv::Mat CreateInpaintMask(const cv::Mat& hsv)
{
	cv::Mat mask;
	mask = cv::Mat::zeros(hsv.size(), hsv.type());

	for (int j = 0; j < hsv.rows; ++j)
	{
		for (int i = 0; i < hsv.cols; ++i)
		{
			cv::Vec3b rpixel = hsv.at<cv::Vec3b>(cv::Point(i, j));

			unsigned val = static_cast<unsigned>(rpixel[2]);

			if (val > 230)
			{
				cv::Vec3b& wpixel = mask.at<cv::Vec3b>(cv::Point(i, j));
				wpixel[2] = 255;
			}
		}
	}

	std::vector<cv::Mat> maskc;
	cv::split(mask, maskc);
	return maskc[2];
}


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

	ZapisDoPliku(gray, name, "1orig");

	// Usuwanie tekstu:
	cv::Mat mask = CreateInpaintMask(hsv);
	cv::inpaint(gray, mask, gray, 2.0, cv::INPAINT_TELEA);
	mask.release();

	// Wygladzanie:
	cv::GaussianBlur(gray, gray, cv::Size(7, 7), 4.5);

	// Krawedzie:

	cv::Mat delt;
	delt.create(gray.size(), gray.type());
	const int kernel_left = 3;
	const int kernel_right = 3;

	for (int j = 0; j < gray.rows; ++j)
	{
		for (int i = 0; i < gray.cols; ++i)
		{
			int& r = reinterpret_cast<int&>(delt.at<uchar>(cv::Point(i, j)));
			int vall, valr;

			if ((i - kernel_left) < 0)
				vall = static_cast<int>(gray.at<uchar>(cv::Point(0, j)));
			else
				vall = static_cast<int>(gray.at<uchar>(cv::Point(i - kernel_left, j)));

			if ((i + kernel_right) >= gray.cols)
				valr = static_cast<int>(gray.at<uchar>(cv::Point(gray.cols-1, j)));
			else
				valr = static_cast<int>(gray.at<uchar>(cv::Point(i + kernel_right, j)));

			const int delta = abs(valr - vall);
			r = delta;
		}
	}


	//ShowWindow(uniq + " Gradient " + name, delt1);

	cv::Mat edges;
	edges.create(gray.size(), gray.type());


	for (int j = 0; j < delt.rows; ++j)
	{
		float sum = 0;
		float avg = 0;

		for (int i = 0; i < delt.cols; ++i)
		{
			const float value = static_cast<float>(delt.at<uchar>(cv::Point(i, j)));

			if (value != 0)
			{
				avg +=	value;
				sum += 1;
			}
		}

		avg = avg / sum;

		for (int i = 0; i < delt.cols; ++i)
		{
			int& r = reinterpret_cast<int&>(edges.at<uchar>(cv::Point(i, j)));
			const float value = static_cast<float>(delt.at<uchar>(cv::Point(i, j)));
			if (value > avg)
				r = 255;
			else
				r = 0;
		}
	}


	cv::Mat dilero;
	edges.copyTo(dilero);

	cv::dilate(dilero, dilero, cv::Mat());
	//cv::dilate(dilero, dilero, cv::Mat());
	cv::erode(dilero, dilero, cv::Mat());
	//cv::erode(dilero, dilero, cv::Mat());

	//ShowWindow(uniq + " Dilero " + name, dilero);
	cv::String n(uniq);
	n += ". " + name;
	MarkEdges(image, dilero, n);



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