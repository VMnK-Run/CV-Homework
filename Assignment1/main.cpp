#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

const float kNoiseProportion = 0.1f;

class Filter {
private:
    Mat image;

public:
    float** getGaussianKernel(int kernel_size=3, float std_dev=1.0) const {
        float **kernel = new float*[kernel_size];
        int kernel_center = kernel_size / 2;
        float sum = 0.0f;
        for (int i = 0; i < kernel_size; i++) {
            kernel[i] = new float[kernel_size];
            for(int j = 0; j < kernel_size; j++){
                float weight = (1 / (2 * CV_PI * pow(std_dev, 2))) * exp(-(pow(i - kernel_center, 2) + pow(j - kernel_center ,2) / 2 * pow(std_dev, 2)));
                sum += weight;
                kernel[i][j] = weight;
            }
        }

        for(int i = 0; i < kernel_size; i++) {
            for(int j = 0; j < kernel_size; j++) {
                kernel[i][j] = kernel[i][j] / sum;
            }
        }
        return kernel;
    }

public:
    Filter(const Mat& image) {
        this->image = image;
    }

    Mat medianFilterMethod(int kernel=3){
        int kernel_center = kernel / 2;
        Mat result = this->image.clone();
        for (int i = 0; i < this->image.rows; i++) {
            for(int j = 0; j < this->image.cols; j++) {
                vector<Vec3b> values;
                uchar r, g, b;
                if(i - kernel_center >= 0 && j - kernel_center >= 0\
                    && i + kernel_center < this->image.rows && j + kernel_center < this->image.cols) {
                    for(int m = -kernel_center; m <= kernel_center; m++) {
                        for(int n = -kernel_center; n <= kernel_center; n++) {
                            values.push_back(this->image.at<Vec3b>(i + m, j + n));
                        }
                    }
                    sort(values.begin(), values.end(), [](Vec3b a, Vec3b b) {return a[0] < b[0];});
                    r = values[kernel * kernel / 2][0];

                    sort(values.begin(), values.end(), [](Vec3b a, Vec3b b) {return a[1] < b[1];});
                    g = values[kernel * kernel / 2][1];

                    sort(values.begin(), values.end(), [](Vec3b a, Vec3b b) {return a[2] < b[2];});
                    b = values[kernel * kernel / 2][2];
                } else {
                    r = this->image.at<Vec3b>(i, j)[0];
                    g = this->image.at<Vec3b>(i, j)[1];
                    b = this->image.at<Vec3b>(i, j)[2];
                }
                result.at<Vec3b>(i, j) = Vec3b(r, g, b);
            }
        }
        return result;
    }

    Mat gaussianFilterMethod(int kernel_size=3, float std_dev=1.0){
        Mat result = this->image.clone();
        int kernel_center = kernel_size / 2;
        float** gaussian_kernel = this->getGaussianKernel(kernel_size, std_dev); // get gaussian kernel
        cout << gaussian_kernel[0][0] << endl;
        for(int i = 0; i < this->image.rows; i++) {
            for(int j = 0; j < this->image.cols; j++) {
                cout << i << " " << j << endl;
                if(i - kernel_center >= 0 && j - kernel_center >= 0\
                    && i + kernel_center < this->image.rows && j + kernel_center < this->image.cols) {
                    // 对三个通道都做滤波
                    for(int c = 0; c < this->image.channels(); c++) {
                        float sum = 0.0f;
                        for(int m = -kernel_center; m <= kernel_center; m++) {
                            for(int n = -kernel_center; n <= kernel_center; n++) {
                                // 获取附近位置的该通道像素值
                                float value = static_cast<float>(this->image.at<Vec3b>(i + m, j + n)[c]);
                                float weight = gaussian_kernel[kernel_center - m][kernel_center - n];  // 卷积
                                sum += value * weight;
                            }
                        }
                        result.at<Vec3b>(i, j)[c] = static_cast<uchar>(sum);
                    }
                } else {
                    result.at<Vec3b>(i, j) = this->image.at<Vec3b>(i, j);
                }
            }
        }
        
        return result;
    }
};

class OpenCVFilter{
private:
    Mat image;

public:
    OpenCVFilter(const Mat& image) {
        this->image = image;
    }

    Mat medianFilterMethod(int kernel_size=3) {
        Mat filter_result;
        medianBlur(this->image, filter_result, kernel_size);
        return filter_result;
    }

    Mat gaussianFilterMethod(int kernel_size=3, float sigma=1.0f) {
        Mat filter_result;
        GaussianBlur(this->image, filter_result, Size(kernel_size, kernel_size), sigma);
        return filter_result;
    }
};

Mat addSaltAndPepper(Mat image) {
    Mat result = image.clone();  // copy original picture
    RNG rng(12345);  // create random number generator
    int cols = image.cols;
    int rows = image.rows;
    int pixel_num = cols * rows;
    int noise_num = pixel_num * kNoiseProportion;
    
    for (int i = 0; i < noise_num; i++) {
        int index_x = rng.uniform(0, cols);
        int index_y = rng.uniform(0, rows);
        if(rand() % 2 == 0) {
            result.at<Vec3b>(index_x, index_y) = Vec3b(0, 0, 0);
        } else {
            result.at<Vec3b>(index_x, index_y) = Vec3b(255, 255, 255);
        }
    }

    return result;
}

Mat addGaussian(Mat image, float mean=10, float std_dev=40) {
    Mat result = image.clone();
    RNG rng(12345);
    Mat gaussian_noise = Mat::zeros(image.rows, image.cols, image.type());

    rng.fill(gaussian_noise, RNG::NORMAL, mean, std_dev);
    result += gaussian_noise;  // 高斯噪声有正有负
    
    return result;
}

void displayHelp(char** argv) {
    cout << "Usage: " << argv[0] << "<noise mode>" << "<filter mode>(optional)" << endl;
    cout << "<noise mode>" << endl;
    cout << "\tdo_gaussian: use gaussian noise and gaussian filter" << endl;
    cout << "\tdo_salt: use salt and pepper noise and median filter" << endl;
    cout << "<filter mode>(optional)" << endl;
    cout << "\topencv: Using OpenCV's built-in functions" << endl;
    cout << "\tour: Using our functions" << endl;
    cout << "\tboth: Using both OpenCV's built-in functions and ours" << endl;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        displayHelp(argv);
        return -1;
    }
    cout << "Assignment1 Running!" << endl;
    Mat image = imread("D:/TJU/22232/ComputerVision/CV-Homework/Assignment1/images/campus.png");
    if(image.empty()) {
        cout << "Image not found" << endl;
        return -1;
    }
    imshow("original campus picture", image);
    string noise_mode = argv[1];
    string filter_mode = argv[2];
    if(noise_mode == "do_salt") {
        Mat salt_pepper_noise = addSaltAndPepper(image);
        imshow("Salt and Pepper noise", salt_pepper_noise);

        if(filter_mode == "opencv") {
            OpenCVFilter opencv_filter = OpenCVFilter(salt_pepper_noise);
            imshow("opencv filter result", opencv_filter.medianFilterMethod());
        } else if(filter_mode == "our") {
            Filter our_filter = Filter(salt_pepper_noise);
            Mat median_filter_result = our_filter.medianFilterMethod();
            imshow("our filter result", median_filter_result);
        } else if(filter_mode == "both") {
            OpenCVFilter opencv_filter = OpenCVFilter(salt_pepper_noise);
            imshow("opencv filter result", opencv_filter.medianFilterMethod());

            Filter our_filter = Filter(salt_pepper_noise);
            Mat median_filter_result = our_filter.medianFilterMethod();
            imshow("our filter result", median_filter_result);
        }

    } else if (noise_mode == "do_gaussian") {
        Mat gaussian_noise = addGaussian(image);
        imshow("Gaussian noise", gaussian_noise);

        if(filter_mode == "opencv") {
            OpenCVFilter opencv_filter = OpenCVFilter(gaussian_noise);
            imshow("opencv filter result", opencv_filter.gaussianFilterMethod());
        } else if(filter_mode == "our") {
            Filter our_filter = Filter(gaussian_noise);
            Mat gaussian_filter_result = our_filter.gaussianFilterMethod();
            imshow("our filter result", gaussian_filter_result);
        } else if(filter_mode == "both") {
            OpenCVFilter opencv_filter = OpenCVFilter(gaussian_noise);
            imshow("opencv filter result", opencv_filter.gaussianFilterMethod());

            Filter our_filter = Filter(gaussian_noise);
            Mat gaussian_filter_result = our_filter.gaussianFilterMethod();
            imshow("our filter result", gaussian_filter_result);
        }

    } else {
        displayHelp(argv);
        return -1;
    }
    waitKey(0);
    cout << "Assignment1 Finished!" << endl;
    return 0;
}