#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "detector.h"
using namespace std;
using namespace cv;

int main(int argc, char** argv) {
	cout << "Assignment2 Running!" << endl;
    Mat src = imread("D:\\TJU\\22232\\ComputerVision\\CV-Homework\\Assignment2\\images\\pic.png");
	if (src.empty()) {
		cout << "Image not found" << endl;
        return -1;
	}
	imshow("Source Image", src);
	OpenCVDetector detector = OpenCVDetector(src);
	// MyDetector detector = MyDetector(src);
	// detector.harriesDetection();
	detector.harriesDetection();

	waitKey(0);
    return 0;
}