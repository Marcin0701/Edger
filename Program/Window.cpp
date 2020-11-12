#include "Window.hpp"


Window::Window(const cv::String& name) : m_name(name)
{
	cv::namedWindow(m_name, cv::WINDOW_AUTOSIZE);
}

void Window::Show(const cv::Mat& image)
{
	cv::imshow(m_name, image);
}