#include <cstdlib>
#include <cstdint>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rpi-arrow-detect.h"

#include "rpi-log.h"

#include <time.h>

using namespace cv;

int RpiArrowDetect::init(uint32_t frameWidth, uint32_t frameHeight)
{
    writeVid_ = cv::VideoWriter("vid.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(frameWidth, frameHeight));
    writeVidContours_ = cv::VideoWriter("vidContours.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(frameWidth, frameHeight));

    width_ = frameWidth;
    height_ = frameHeight;

    return 0;
}

int RpiArrowDetect::detectArrow(uint8_t *frame, size_t size, int &x, int &y) {
    Point arrowTipInImage = Point(-1, -1);
    Point arrowCenterInImage = Point(-1, -1);

    Mat cvFrame(height_, width_, CV_8UC3, frame);
    Mat hsv;
    cvtColor(cvFrame, hsv, COLOR_BGR2HSV);

    // Define the green color range (adjust based on your arrow color)
    Scalar lowerOrange(10, 50, 50);  // Lower bound of green in HSV
    Scalar upperOrange(20, 255, 255); // Upper bound of green in HSV

    Mat mask;
    // Threshold the HSV image to get only green colors
    inRange(hsv, lowerOrange, upperOrange, mask);

    Mat processed;
    // Morphological operations to remove noise
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, processed, MORPH_CLOSE, kernel);
    morphologyEx(processed, processed, MORPH_OPEN, kernel);

    // Find contours in the processed image
    std::vector<std::vector<Point>> contours;
    findContours(processed, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    double biggestContourArea = 300;
    for (size_t i = 0; i < contours.size(); i++) {
        // Approximate the contour to reduce the number of points
        std::vector<Point> approx;
        approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.01, true);

        // Check if the contour has enough vertices to resemble an arrow shape
        if (approx.size() == 7) { // Typical arrows have 7-10 corners
            // Optionally, filter by area to ignore small shapes
            double area = contourArea(approx);
            if (area > biggestContourArea) { // Adjust this threshold based on the expected size of the arrows
                // Calculate the centroid of the arrow
                biggestContourArea = area;
                Moments M = moments(approx);
                arrowCenterInImage.x = int(M.m10 / M.m00);
                arrowCenterInImage.y = int(M.m01 / M.m00);

                drawContours(cvFrame, std::vector<std::vector<Point>>{approx}, -1, Scalar(0, 255, 0), 2);
                circle(cvFrame, arrowCenterInImage, 5, Scalar(255, 0, 0), FILLED); // Center

                // Print the coordinates of the centroid
                RPI_LOG("RpiArrowDetect", ERROR, "Arrow detected at: %d, %d, approx size %d", arrowCenterInImage.x, arrowCenterInImage.y, approx.size());// std::cout << "Arrow detected at: (" << cX << ", " << cY << ")" << " approx size : " << approx.size() << std::endl;
            }
        }
    }

    writeVid_ << cvFrame;

    if (arrowCenterInImage.x != -1 && arrowCenterInImage.y != -1) {
        x = arrowCenterInImage.x;
        y = arrowCenterInImage.y;
    }
}

int RpiArrowDetect::deinit()
{
    writeVid_.release();
    writeVidContours_.release();
}