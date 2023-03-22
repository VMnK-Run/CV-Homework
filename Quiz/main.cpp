#include "opencv2/core/core.hpp"  
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include <iostream>    

using namespace cv;
using namespace std;

Mat image;
bool leftButtonDownFlag = false;

Point originalPoint; 
Point processPoint;

Mat targetImageHSV;
int histSize = 200;
float histR[] = {0, 255};
const float *histRange = histR;
int channels[] = {0, 1};
Mat dstHist;
Rect rect;
vector<Point> pt; //保存目标轨迹
RotatedRect track_box;
void onMouse(int event, int x, int y, int flags, void* ustc); //鼠标回调函数  

int main(int argc, char** argv) {
    if(argc < 2) {
        cout << "Please enter the mode you want to run: mean or cam." << endl;
        return -1;
    }
    string mode = argv[1];
	VideoCapture video("D:\\TJU\\22232\\ComputerVision\\CV-Homework\\Quiz\\videos\\video.mp4");

	namedWindow("Tracking", 0);
	setMouseCallback("Tracking", onMouse);
    
	while (true) {
		if (!leftButtonDownFlag) {
            video >> image;
		} else {
            waitKey(0);
        }

        if(image.empty()) {
            break;
        }

		if (originalPoint != processPoint && !leftButtonDownFlag) {
			Mat imageHSV;
			Mat calcBackImage;
			cvtColor(image, imageHSV, COLOR_RGB2HSV);
			calcBackProject(&imageHSV, 2, channels, dstHist, calcBackImage, &histRange);  //反向投影
			TermCriteria criteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 1000, 0.001);
            if(mode == "mean") {
                cv::meanShift(calcBackImage, rect, criteria);
            } else if(mode == "cam") {
                cv::CamShift(calcBackImage, rect, criteria);
            } 
			Mat imageROI = imageHSV(rect);   //更新模板			
			targetImageHSV = imageHSV(rect);
			calcHist(&imageROI, 2, channels, Mat(), dstHist, 1, &histSize, &histRange);
			normalize(dstHist, dstHist, 0.0, 1.0, NORM_MINMAX);   //归一化
			rectangle(image, rect, Scalar(0, 0, 255), 3);  	//目标绘制	
			pt.push_back(Point(rect.x + rect.width / 2, rect.y + rect.height / 2));
			for (int i = 0; i < pt.size() - 1; i++) {
				line(image, pt[i], pt[i + 1], Scalar(0, 255, 0), 2.5);
			}
		}
		imshow("Tracking", image);
		if(waitKey(10) == 27) {
            break;
        }
	}
	return 0;
}

void onMouse(int event, int x, int y, int flags, void *ustc) {
	if (event == EVENT_LBUTTONDOWN) {
		leftButtonDownFlag = true; 
		originalPoint = Point(x, y);
		processPoint = originalPoint;
	}
	if (event == EVENT_MOUSEMOVE && leftButtonDownFlag) {
		Mat imageCopy = image.clone();
		processPoint = Point(x, y);
		if (originalPoint != processPoint) {
			rectangle(imageCopy, originalPoint, processPoint, Scalar(0, 0, 255), 2);
		}
		imshow("Tracking", imageCopy);
	}
	if (event == EVENT_LBUTTONUP) {
		leftButtonDownFlag = false;
		rect = Rect(originalPoint, processPoint);
		Mat rectImage = image(rect); //子图像显示  
		imshow("Sub Image", rectImage);
		cvtColor(rectImage, targetImageHSV, COLOR_RGB2HSV);
		calcHist(&targetImageHSV, 2, channels, Mat(), dstHist, 1, &histSize, &histRange, true, false);
		normalize(dstHist, dstHist, 0, 255, NORM_MINMAX);
	}
}
