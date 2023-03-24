#include <iostream>
#include "opticalFlow.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    cout << "Assignment3 Running!" << endl;

    string fileName = "D:\\TJU\\22232\\ComputerVision\\CV-Homework\\Assignment3\\videos\\vtest.avi";
    OpticalFlowCalculator opticalFlowCalculator(fileName);
    opticalFlowCalculator.opencvLK();

    // opticalFlowCalculator.myLK();

    cout << "Assignment3 Finished!" << endl;
    return 0;
}