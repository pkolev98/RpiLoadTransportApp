#pragma once

#include <cstdlib>
#include <cstdint>
#include <memory>

#include "rpi-cam.h"
#include "rpi-arrow-detect.h"
#include "rpi-motor-rover.h"

#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 720

class RpiControl {
public:
    void camFrameComplete(uint8_t *rpiBuf, size_t size);

    RpiControl() {
        rpiArrowDetect_ = std::make_unique<RpiArrowDetect>();
        rpiCamInst_ = std::make_unique<RpiCamera>();
        rpiRover_ = std::make_unique<RpiMotorRover>();
    }

#ifdef WRITE_VID_DEBUG
    void write_vid();
    std::queue<uint8_t *> camBufs_;
#endif

    void init();

    void start();

    int processRequest();

    void stop();

    ~RpiControl() {
        rpiArrowDetect_ = nullptr;
        rpiCamInst_ = nullptr;
        rpiRover_ = nullptr;
    }

private:
    std::unique_ptr<RpiArrowDetect> rpiArrowDetect_;
    std::unique_ptr<RpiCamera> rpiCamInst_;
    std::unique_ptr<RpiMotorRover> rpiRover_;
};