#include "detector.h"

void OpenCVDetector::harriesDetection() {
    Mat result = this->src.clone();
    Mat gray, norm_dst;
    Mat dst = Mat::zeros(this->src.size(), CV_32FC1);
    cvtColor(this->src, gray, COLOR_BGR2GRAY);

    int block_size = 2;  // 邻域大小
    int ksize = 3; // 索贝尔算子参数值
    double k = 0.04;  // 阈值

    cv::cornerHarris(gray, dst, block_size, ksize, k, BORDER_DEFAULT);
    normalize(dst, norm_dst, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

    for(int i = 0; i < dst.rows; i++) {
        for(int j = 0; j < dst.cols; j++) {
            if(norm_dst.at<float>(i, j) > 150) {
                Point pt(j, i);
                circle(result, pt, 2, Scalar(0, 255, 0), 2, 8, 0);
            }
        }
    }

    imshow("result", result);
}

void MyDetector::harriesDetection() {
    Mat gray;
    cvtColor(this->src, gray, COLOR_BGR2GRAY);

    Mat dx, dy;
    cv::Sobel(gray, dx, CV_32FC1, 1, 0);
    cv::Sobel(gray, dy, CV_32FC1, 0, 1);

    Mat M = Mat::zeros(gray.size(), CV_32FC3);
    for (int i = 0; i < gray.rows; i++) {
        for (int j = 0; j < gray.cols; j++) {
            float Ix = dx.at<float>(i, j);
            float Iy = dy.at<float>(i, j);
            M.at<Vec3f>(i, j) = Vec3f(Ix * Ix, Iy * Iy, Ix * Iy);
        }
    }

    GaussianBlur(M, M, Size(5, 5), 2, 2);

    Mat R = Mat::zeros(gray.size(), CV_32F);
    for (int i = 0; i < gray.rows; i++) {
        for (int j = 0; j < gray.cols; j++) {
            float A = M.at<Vec3f>(i, j)[0];
            float B = M.at<Vec3f>(i, j)[1];
            float C = M.at<Vec3f>(i, j)[2];

            float detM = A * B - C * C;
            float traceM = A + B;
            
            R.at<float>(i, j) = detM - 0.05 * traceM * traceM;
        }
    }

    for(int i = 2; i < R.rows; i++) {
        for (int j = 2; j < R.cols; j++) {
            for (int k = i - 2; k <= i + 2; k++)
                for (int l = j - 2; l <= j + 2; l++)
                    if (R.at<float>(k, l) <= R.at<float>(i, j) && k != i && l != j && R.at<float>(k, l) > 0) {
                        R.at<float>(i, j) = 0;    
                    }
        }
    }

    normalize(R, R, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

    Mat result = this->src.clone();
    for(int i = 0; i < R.rows; i++) {
        for(int j = 0; j < R.cols; j++) {
            if(R.at<float>(i, j) > 150) {
                Point pt(j, i);
                circle(result, pt, 2, Scalar(0, 255, 0), 2, 8, 0);
            }
        }
    }

    imshow("result", result);
}

void OpenCVDetector::SIFTDetection(){
    // 初始化 SIFT 对象，提取特征点
    Ptr<SIFT> detector = SIFT::create();
    vector<KeyPoint> keypoints;
    detector->detect(this->src, keypoints);

    // 提取特征点的描述符
    Mat descriptors;
    detector->compute(this->src, keypoints, descriptors);

    Mat img_keypoints;
    drawKeypoints(this->src, keypoints, img_keypoints, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    imshow("result", img_keypoints);

}

void MyDetector::SIFTDetection() {

}