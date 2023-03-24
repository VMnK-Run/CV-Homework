#ifndef __OPTICALFLOW_H__
#define __OPTICALFLOW_H__

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using std::string;
using std::vector;

class OpticalFlowCalculator {
private:
    std::string fileName;

public:
    OpticalFlowCalculator(const std::string fileName) : fileName(fileName) {}
    void opencvLK();
    void myLK();
};

#endif // __OPTICALFLOW_H__