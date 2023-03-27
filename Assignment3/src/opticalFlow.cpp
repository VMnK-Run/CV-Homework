#include "opticalFlow.h"

void OpticalFlowCalculator::opencvLK() {
	VideoCapture capture(this->fileName);
	namedWindow("OpenCV LK", 0);
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

void OpticalFlowCalculator::myLK(int level, int windowSize) {
	namedWindow("My LK", 0);
	VideoCapture capture(this->fileName);
	int width = capture.get(CAP_PROP_FRAME_WIDTH);
	int height = capture.get(CAP_PROP_FRAME_HEIGHT);

	Mat prevFrame, prevGray;
	Mat nextFrame, nextGray;
	std::vector<Point2f> prevPoints, nextPoints;

	capture >> prevFrame;
	cvtColor(prevFrame, prevGray, COLOR_BGR2GRAY);

	// 初始特征点选择
	cv::goodFeaturesToTrack(prevGray, prevPoints, 100, 0.3, 7, Mat(), 7);
	std::vector<Point2f> testPoints;
	for(int i = 0; i < prevPoints.size(); i++) {
		// cout << i << " " << prevPoints[i] << endl;
		testPoints.push_back(prevPoints[i]);
	}

	
	// testPoints.push_back(prevPoints[0]);

	Mat displayMat;
	Mat mask = Mat::zeros(prevFrame.size().height, prevFrame.size().width, CV_8UC3);

	while (true) {
		capture >> nextFrame;
		if (nextFrame.empty()) {
			break;
		}
		cvtColor(nextFrame, nextGray, COLOR_BGR2GRAY);
		
		nextPoints = this->calcLKOpticalFlow(prevGray, nextGray, testPoints, level, windowSize);
		for(int i = 0; i < nextPoints.size(); i++) {
			cv::Point2f nextPoint = nextPoints[i];
			cv::Point2f prevPoint = testPoints[i];
			std::cout << i << " " << prevPoint << " ==> " << nextPoint << " " << std::endl;
			cv::line(mask, nextPoint, prevPoint, Scalar(0, 255, 0), 2, LINE_AA);
			cv::circle(nextFrame, nextPoint, 3, Scalar(0, 0, 255), -1);
		}
		// break;
		cv::add(nextFrame, mask, displayMat);
		imshow("My LK", displayMat);
		if(waitKey(1) == 27) {
			break;
		}
		prevFrame = nextFrame.clone();
		prevGray = nextGray.clone();
		std::copy(nextPoints.begin(), nextPoints.end(), testPoints.begin());
	}
}

// 计算相邻两帧之间的光流，利用金字塔迭代
std::vector<Point2f> OpticalFlowCalculator::calcLKOpticalFlow(cv::InputArray prevImg, cv::InputArray nextImg, std::vector<Point2f> prevPoints, int level, int windowsize) {
	cout << "=====Calc LKOptical=====" << endl;
	std::vector<Point2f> nextPoints;
	std::vector<cv::Mat> prevPyramids = this->buildPyramid(prevImg, level);
	std::vector<cv::Mat> nextPyramids = this->buildPyramid(nextImg, level);
	// 计算每一个特征点的光流
	for(int i = 0; i < prevPoints.size(); i++) {
		bool isValid = true;
		cv::Point2f prevPoint = prevPoints[i];
		cv::Mat g(2, 1, CV_64F, Scalar(0)); // 估计值，初始为 0
		for (int j = level - 1; j >= 0; j--) {
			cv::Mat prevMat = prevPyramids[j];
			cv::Mat nextMat = nextPyramids[j];
			prevMat.convertTo(prevMat, CV_64F);
			nextMat.convertTo(nextMat, CV_64F);

			cv::Point2f point;
			point.x = 1.0 * prevPoint.x / pow(2, j);
			point.y = 1.0 * prevPoint.y / pow(2, j);
			Mat gradient = this->getGradientMat(prevMat, point, windowsize);

			Mat v(2, 1, CV_64F, Scalar(0));  // 残差修正，需要在每一层中根据 v = G^-1 * b计算
			
			int iterNum = 0;
			double residual = 1;
			// std::cout << std::endl;
			// std::cout << "=======" << std::endl;
			while(iterNum < this->maxIterations && residual > this->residualThreshold) {
				iterNum++;
				cv::Mat b = this->getMismatchVector(prevMat, nextMat, point.x, point.y, g, v, windowsize);
				Mat miu = gradient.inv() * b;
				v += miu;  // v 只在这里更新
				residual = this->getResidual(miu);
				// cout << "level = " << j << " iter = " << iterNum << " b = " << b.at<double>(0, 0) << " " << b.at<double>(1, 0) <<std::endl;
				// std::cout << "residual= " << residual << std::endl; 
			}
			// cout << "level = " << j << " v = " << v.at<double>(0, 0) << " " << v.at<double>(1, 0) <<std::endl;
			if (j == 0) {
				g = g + v;  // 不再向下一层传播
			} else {
				g = 2 * (g + v);  // g 是估计值，只在这个地方更新
			}
		}
		// 此时, g 已经是最终计算结果
		// std::cout << g << std::endl;
		if(abs(g.at<double>(0, 0)) < 20 && abs(g.at<double>(1, 0)) < 20) {
			Point nextPoint;
			nextPoint.x = (prevPoint.x + g.at<double>(1, 0));
			nextPoint.y = (prevPoint.y + g.at<double>(0, 0));
			nextPoints.push_back(nextPoint);
		} else {
			nextPoints.push_back(prevPoint);
		}
		
		// } else {
		// 	nextPoints.push_back(prevPoint);
		// }
	}
	return nextPoints;
}

cv::Mat OpticalFlowCalculator::getGradientMat(cv::Mat frame, Point point, int windowSize) {
	Mat dx, dy;
	cv::Sobel(frame, dx, CV_64F, 1, 0);
	cv::Sobel(frame, dy, CV_64F, 0, 1);

	Mat gradient(2, 2, CV_64F, Scalar(0));
	for(int i = point.y - windowSize; i <= point.y + windowSize; i++) {
		for (int j = point.x - windowSize; j <= point.x + windowSize; j++) {
			// double Ix = this->getMatDouble(dx, i, j);
			// double Iy = this->getMatDouble(dy, i, j);
			double Ix = (this->getMatDouble(frame, i + 1, j) - this->getMatDouble(frame, i - 1, j)) / 2.0;
			double Iy = (this->getMatDouble(frame, i, j + 1) - this->getMatDouble(frame, i, j - 1)) / 2.0;
			gradient.at<double>(0, 0) += Ix * Ix;
			gradient.at<double>(0, 1) += Ix * Iy;
			gradient.at<double>(1, 0) += Ix * Iy;
			gradient.at<double>(1, 1) += Iy * Iy;
		}
	}
	return gradient;
}

double OpticalFlowCalculator::getResidual(cv::Mat input) {
	return sqrt(pow(input.at<double>(0, 0), 2) + pow(input.at<double>(1, 0), 2));
}

// 计算残差 b，参数 g 和 v 用于预先平移 prevMat
cv::Mat OpticalFlowCalculator::getMismatchVector(cv::Mat prevFrame, cv::Mat nextFrame, int x, int y, cv::Mat g, cv::Mat v, int windowSize) {
	cv::Mat b(2, 1, CV_64F, Scalar(0));
	Mat dx, dy;
	cv::Sobel(prevFrame, dx, CV_64F, 1, 0);
	cv::Sobel(prevFrame, dy, CV_64F, 0, 1);
	for(int i = y - windowSize; i <= y + windowSize; i++) {
		for(int j = x - windowSize; j <= x + windowSize; j++) {
			double curX = i + g.at<double>(0, 0) + v.at<double>(0, 0);
			double curY = j + g.at<double>(1, 0) + v.at<double>(1, 0);
			double deltaI = this->getMatDouble(prevFrame, i, j) - this->getMatDouble(nextFrame, curX, curY);
			// cout << "deltaI = " << deltaI << endl;
			double Ix = (this->getMatDouble(prevFrame, i + 1, j) - this->getMatDouble(prevFrame, i - 1, j)) / 2.0;
			double Iy = (this->getMatDouble(prevFrame, i, j + 1) - this->getMatDouble(prevFrame, i, j - 1)) / 2.0;
			b.at<double>(0, 0) += deltaI * Ix;
			b.at<double>(1, 0) += deltaI * Iy;
		}
	}
	// cout << "b: " << b.at<double>(0, 0)  << " " <<b.at<double>(1, 0) << endl;
	return b;
}

int OpticalFlowCalculator::getMatInt(cv::Mat frame, int x, int y) {
	if(x < 0 || y < 0 || x >= frame.size().height || y >= frame.size().width) {
		return 0;
	} else {
		return (int)(frame.at<uchar>(x, y));
	}
}

double OpticalFlowCalculator::getMatDouble(cv::Mat frame, int x, int y) {
	if(x < 0 || y < 0 || x >= frame.size().height || y >= frame.size().width) {
		return 0;
	} else if(frame.type() == CV_64F) {
		return frame.at<double>(x, y);
	} else{
		return (double)frame.at<uchar>(x, y);
	}
}

double OpticalFlowCalculator::getMatDouble(cv::Mat frame, double x, double y) {
	if(x < 0 || y < 0 || x >= frame.size().height || y >= frame.size().width) {
		return 0;
	}
	int floorRow = floor(x);
	int floorCol = floor(y);
	double fracRow = x - floorRow;
	double fracCol = y - floorCol;
	int ceilRow = floorRow + 1;
	int ceilCol = floorCol + 1;

	return ((1.0 - fracRow) * (1.0 - fracCol) * getMatDouble(frame, floorRow, floorCol) + 
		(fracRow * (1.0 - fracCol) * getMatDouble(frame, ceilRow, floorCol)) + 
		((1.0 - fracRow) * fracCol * getMatDouble(frame, floorRow, ceilCol)) +
		(fracRow * fracCol * getMatDouble(frame, ceilRow, ceilCol))
	);
}

cv::Mat OpticalFlowCalculator::getLowPassFrame(cv::Mat frame) {
	int width = frame.size().width / 2;
	int height = frame.size().height / 2;
	Mat nextFrame;
	nextFrame.create(Size(width, height), frame.type());
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			double val = 0;
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					int matVal = this->getMatInt(frame, 2 * x + i, 2 * y + j);
					if (i == 0 && j == 0) {
						val += 0.25 * matVal;
					} else if (i == 0 || j == 0) {
						val += 0.125 * matVal;
					} else {
						val += 0.0625 * matVal;
					}
				}
			}
			nextFrame.at<uchar>(x, y) = val;
		}
	}
	return nextFrame;
}

std::vector<cv::Mat> OpticalFlowCalculator::buildPyramid(cv::InputArray frame, int level) {
	std::vector<cv::Mat> pyramids;
	Mat curFrame = frame.getMat().clone();
	pyramids.push_back(curFrame);
	for (int i = 1; i < level; i++) {
		Mat nextFrame = this->getLowPassFrame(curFrame);
		pyramids.push_back(nextFrame.clone());
		curFrame = nextFrame.clone();
	}
	return pyramids;
}