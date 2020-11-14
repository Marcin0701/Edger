#include "Window.hpp"


Window::Window(const cv::String& name, const cv::Mat& image) : m_name(name), m_image(image)
{
	cv::namedWindow(m_name, cv::WINDOW_AUTOSIZE);
}

void Window::Show()
{
	cv::imshow(m_name, m_image);
}