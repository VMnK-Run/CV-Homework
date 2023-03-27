#ifndef __OPTICALFLOW_H__
#define __OPTICALFLOW_H__

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using std::cout;
using std::endl;
using std::string;
using std::vector;

class OpticalFlowCalculator {
private:
    std::string fileName;
    int maxIterations = 50;
    double residualThreshold = 0.001;
    double getResidual(cv::Mat input);
    cv::Mat getLowPassFrame(cv::Mat frame);
    int getMatInt(cv::Mat frame, int x, int y);
    double getMatDouble(cv::Mat frame, int x, int y);
    double getMatDouble(cv::Mat frame, double x, double y);
    cv::Mat getGradientMat(cv::Mat frame, Point point, int windowSize);
    cv::Mat getMismatchVector(cv::Mat prevFrame, cv::Mat nextFrame, int x, int y, cv::Mat g, cv::Mat v, int windowSize);
    std::vector<cv::Mat> buildPyramid(cv::InputArray frame, int level);
    std::vector<Point2f> calcLKOpticalFlow(cv::InputArray prevImg, cv::InputArray nextImg, std::vector<Point2f> prevPoints, int level, int widowsize);

public:
    OpticalFlowCalculator(const std::string fileName) : fileName(fileName) {}
    void opencvLK();
    void myLK(int level=3, int windowSize=1);
};

#endif // __OPTICALFLOW_H__