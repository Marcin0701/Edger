#pragma once

#include <opencv2/opencv.hpp>


class Window
{
public:
	Window(const cv::String& name);
	void Show(const cv::Mat& image);
private:
	cv::String m_name;
};