#pragma once

#include <cstdlib>
#include <cstdint>

#include "rpi-cam.h"
#include "rpi-arrow-detect.h"
#include "rpi-motor-rover.h"

#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 720

class RpiControl {
public:
    void camFrameComplete(uint8_t *rpiBuf, size_t size);

    RpiControl() {}

#ifdef WRITE_VID_DEBUG
    void write_vid();
    std::queue<uint8_t *> camBufs_;
#endif

    void init();

    void start();

    int processRequest();

    void stop();

    ~RpiControl() {}

private:
    RpiArrowDetect rpiArrowDetect_;
    RpiCamera rpiCamInst_;
    RpiMotorRover rpiRover_;
};