#include <iostream>
#include "opticalFlow.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    if(argc < 2) {
        cout << "Please enter the mode you want to run: opencv or my." << endl;
        return -1;
    }
    string mode = argv[1];
    cout << "Assignment3 Running!" << endl;

    string fileName = "D:\\TJU\\22232\\ComputerVision\\CV-Homework\\Assignment3\\videos\\video.mp4";
    OpticalFlowCalculator opticalFlowCalculator(fileName);
    if(mode == "opencv") {
        opticalFlowCalculator.opencvLK();
    } else if(mode == "my") {
        opticalFlowCalculator.myLK();
    }

    cout << "Assignment3 Finished!" << endl;
    return 0;
}