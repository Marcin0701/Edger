#pragma once

#include <opencv2/opencv.hpp>


class Window
{
public:
	Window(const cv::String& name, const cv::Mat& image);
	void Show();
private:
	cv::String m_name;
	cv::Mat m_image;
};