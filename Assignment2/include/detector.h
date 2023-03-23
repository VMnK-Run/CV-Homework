#ifndef __TEST_H__
#define __TEST_H__

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class OpenCVDetector{
private:
    cv::Mat src;

public:
    OpenCVDetector(cv::Mat src) : src(src) {}
    void harriesDetection(int threshold=130);
    void SIFTDetection();
};

class MyDetector{
private:
    cv::Mat src;

public:
    MyDetector(cv::Mat src) : src(src) {}
    void harriesDetection(int threshold=110);
};

#endif