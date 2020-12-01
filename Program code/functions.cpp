#include "functions.hpp"


void WindowShow(const cv::String& name, const cv::Mat& img)
{
	cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
	cv::imshow(name, img);
}

void FindEdges(const cv::Mat& gray_in, cv::Mat& gray_out, const int kleft, const int kright)
{
	cv::Mat delt;
	delt.create(gray_in.size(), gray_in.type());

	for (int j = 0; j < gray_in.rows; ++j)
	{
		for (int i = 0; i < gray_in.cols; ++i)
		{
			int& r = reinterpret_cast<int&>(delt.at<uchar>(cv::Point(i, j)));
			int vall, valr;

			if ((i - kleft) < 0)
				vall = static_cast<int>(gray_in.at<uchar>(cv::Point(0, j)));
			else
				vall = static_cast<int>(gray_in.at<uchar>(cv::Point(i - kleft, j)));

			if ((i + kright) >= gray_in.cols)
				valr = static_cast<int>(gray_in.at<uchar>(cv::Point(gray_in.cols - 1, j)));
			else
				valr = static_cast<int>(gray_in.at<uchar>(cv::Point(i + kright, j)));

			const int delta = cv::abs(valr - vall);
			r = delta;
		}
	}

	if(gray_out.empty())
		gray_out.create(gray_in.size(), gray_in.type());

	for (int j = 0; j < delt.rows; ++j)
	{
		float sum = 0;
		float avg = 0;

		for (int i = 0; i < delt.cols; ++i)
		{
			const float value = static_cast<float>(delt.at<uchar>(cv::Point(i, j)));

			if (value != 0)
			{
				avg += value;
				sum += 1;
			}
		}

		avg = avg / sum;

		for (int i = 0; i < delt.cols; ++i)
		{
			int& r = reinterpret_cast<int&>(gray_out.at<uchar>(cv::Point(i, j)));
			const float value = static_cast<float>(delt.at<uchar>(cv::Point(i, j)));
			if (value > avg)
				r = 255;
			else
				r = 0;
		}
	}
}

std::vector<cv::Rect> FindRects(const cv::Mat& gray)
{
	const float dens_resolution = 0.9f;

	std::vector<int> lines;
	lines.reserve(gray.cols);

	// Pszeszukuj linie pionowe o jak najwiekszej liczbie bialych pikseli:
	for (int i = 0; i < gray.cols; ++i)
	{
		int line_density = 0;

		for (int j = 0; j < gray.rows; ++j)
		{
			const unsigned value = static_cast<unsigned>(gray.at<uchar>(cv::Point(i, j)));
			if (value)
				line_density += 1;
		}

		if (static_cast<float>(line_density) > static_cast<float>(gray.rows * dens_resolution))
			lines.push_back(i);
	}


	const int max_linerows = 10;
	const int min_rect_width = 5;
	int line_rows = 0;
	std::vector<cv::Rect> rects;
	int temp = lines[0];

	for (const auto& l : lines)

		for (int i = 1; i < lines.size(); ++i)
		{
			// Badanie roznicy odleglosci dwoch linii:
			const int delta = lines[i] - lines[i - 1];

			if (delta > max_linerows || i == lines.size() - 1)
			{
				const int rect_width = lines[i - 1] - temp;

				if (rect_width >= min_rect_width)
					rects.push_back(cv::Rect(temp, 0, lines[i - 1] - temp, gray.rows));
				const int j = i + 1;
				if (j < lines.size())
				{
					temp = lines[i];
				}
			}
		}

	return rects;
}


void MarkEdges(cv::Mat& gray, const std::vector<cv::Rect>& rects)
{
	for (const auto& r : rects)
		cv::rectangle(gray, r.tl(), r.br(), cv::Scalar(0, 255, 0), 2, 8, 0);
}

void RemoveText(const cv::Mat& gray_in, cv::Mat& gray_out)
{
	cv::Mat mask;
	mask = cv::Mat::zeros(gray_in.size(), gray_in.type());

	for (int j = 0; j < gray_in.rows; ++j)
	{
		for (int i = 0; i < gray_in.cols; ++i)
		{
			const unsigned val = static_cast<unsigned>(gray_in.at<uchar>(cv::Point(i, j)));

			if (val > 230)
			{
				unsigned& r = reinterpret_cast<unsigned&>(mask.at<uchar>(cv::Point(i, j)));
				r = 255;
			}
		}
	}

	cv::inpaint(gray_in, mask, gray_out, 2.0, cv::INPAINT_TELEA);
}