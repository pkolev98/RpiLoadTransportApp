#include <cstdlib>
#include <cstdint>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "rpi-arrow-detect.h"

using namespace cv;

void RpiArrowDetect::setFramesRes(uint32_t width, uint32_t height) {
    width_ = width;
    height_ = height;
}

int RpiArrowDetect::detectArrow(uint8_t *frame, size_t size, int &x, int &y) {
    Mat cvFrame(1080, 1920, CV_8UC3, frame);
    
    Mat gray;
    cvtColor(cvFrame, gray, COLOR_BGR2GRAY);

    // Step 3: Apply Gaussian Blur to reduce noise
    Mat blurred;
    GaussianBlur(gray, blurred, Size(5, 5), 0);

    // Step 4: Perform Canny edge detection
    Mat edges;
    Canny(blurred, edges, 100, 200);

    // Step 5: Find contours
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(edges, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Variable to store the coordinates of the arrow
    Point arrowCenter(-1, -1);

    // Step 6: Iterate over the contours and analyze the shapes
    for (size_t i = 0; i < contours.size(); i++) {

        // Approximate each contour to a polygon
        std::vector<Point> approx;
        double epsilon = 0.02 * arcLength(contours[i], true);
        approxPolyDP(contours[i], approx, epsilon, true);

        // Filter based on the number of vertices and convexity
        if (approx.size() >= 7 && isContourConvex(approx)) {
            // Compute the bounding rectangle of the polygon (optional)
            Rect boundingBox = boundingRect(approx);

            // Simple rule: arrows generally have an aspect ratio > 1
            double aspectRatio = (double)boundingBox.width / boundingBox.height;
            if (aspectRatio > 1.2) {
                // Step 7: Calculate the centroid of the arrow
                Moments M = moments(approx);
                if (M.m00 != 0) {
                    arrowCenter = Point(int(M.m10 / M.m00), int(M.m01 / M.m00));
                }
            }
        }
    }

    if (arrowCenter.x != -1 && arrowCenter.y != -1) {
        x = arrowCenter.x;
        y = arrowCenter.y;
    }

    return 0;
}