#include "opticalFlow.h"

void OpticalFlowCalculator::opencvLK() {
	VideoCapture capture(this->fileName);
	int width = capture.get(CAP_PROP_FRAME_WIDTH);
	int height = capture.get(CAP_PROP_FRAME_HEIGHT);

	Mat prevFrame, prevGray;
	capture >> prevFrame;
	cvtColor(prevFrame, prevGray, COLOR_BGR2GRAY);

	TermCriteria criteria(TermCriteria::COUNT | TermCriteria::EPS, 10, 0.03);

	std::vector<Point2f> prevPoints, nextPoints;
	std::vector<uchar> status;  // 是否成功
	std::vector<float> err;

	cv::goodFeaturesToTrack(prevGray, prevPoints, 100, 0.3, 7, Mat(), 7);

	// 线段需要持续绘制
	Mat mask = Mat::zeros(prevFrame.size().height, prevFrame.size().width, CV_8UC3);
	Mat displayFrame;

	Mat nextFrame, nextGray;
	while(true) {
		capture >> nextFrame;
		if(nextFrame.empty()) {
			break;
		}
		cvtColor(nextFrame, nextGray, COLOR_BGR2GRAY);

		cv::calcOpticalFlowPyrLK(prevGray, nextGray, prevPoints, nextPoints, status, err, Size(15, 15), 3, criteria);

		for(int i = 0; i < nextPoints.size(); i++) {
			cv::Point2f nextPoint = nextPoints[i];
			cv::Point2f prevPoint = prevPoints[i];
			cv::line(mask, nextPoint, prevPoint, Scalar(0, 255, 0), 2, LINE_AA);
			cv::circle(nextFrame, nextPoint, 3, Scalar(0, 0, 255), -1);
		}

		cv::add(nextFrame, mask, displayFrame);
		imshow("OpenCV LK", displayFrame);
		if(waitKey(10) == 27) {
			break;
		}

		prevFrame = nextFrame.clone();
		prevGray = nextGray.clone();
		std::copy(nextPoints.begin(), nextPoints.end(), prevPoints.begin());
	}
	capture.release();
}

void OpticalFlowCalculator::myLK() {

}