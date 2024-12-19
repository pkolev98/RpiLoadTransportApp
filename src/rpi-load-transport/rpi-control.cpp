#include <cstdlib>
#include <cstdint>

#include "rpi-control.h"

#include "rpi-log.h"

void RpiControl::init() {
    rpiCamInst_->init();
    rpiCamInst_->rpiRequestComplete = std::bind(&RpiControl::camFrameComplete, this, std::placeholders::_1, std::placeholders::_2);
    rpiCamInst_->configure(FRAME_WIDTH, FRAME_HEIGHT, RpiCamera::RpiCameraFormat::RGB888);
    rpiCamInst_->allocBuffers();

    rpiArrowDetect_->init(FRAME_WIDTH, FRAME_HEIGHT);

    RpiMotorRover::MRPoint targetPoint;
    targetPoint.x = FRAME_WIDTH / 2;
    targetPoint.y = FRAME_HEIGHT / 2;
    rpiRover_->init(targetPoint);

}

void RpiControl::start() {
    rpiCamInst_->start();
    rpiRover_->start();
}

int RpiControl::processRequest() {
    return rpiCamInst_->processRequest();
}

void RpiControl::stop() {
    rpiCamInst_->stop();
    rpiCamInst_->deinit();
    rpiRover_->stop();
    rpiRover_->deinit();
    rpiArrowDetect_->deinit();
}