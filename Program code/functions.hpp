#pragma once

#include <opencv2/opencv.hpp>
#include <vector>


void WindowShow(const cv::String& name, const cv::Mat& img);
void FindEdges(const cv::Mat& gray_in, cv::Mat& gray_out, const int kleft = 3, const int kright = 3);
std::vector<cv::Rect> FindRects(const cv::Mat& gray);
void MarkEdges(cv::Mat& gray, const std::vector<cv::Rect>& rects);
void RemoveText(const cv::Mat& gray_in, cv::Mat& gray_out);