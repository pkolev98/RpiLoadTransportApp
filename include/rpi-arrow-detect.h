#pragma once

#include <cstdint>
#include <cstdlib>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

class RpiArrowDetect {
public:
    void setFramesRes(uint32_t width, uint32_t height);

    int detectArrow(uint8_t *frame, size_t size, int &x, int &y);

private:
    uint32_t width_;
    uint32_t height_;
};