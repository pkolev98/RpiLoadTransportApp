#include <cstdlib>
#include <cstdint>

#include "rpi-control.h"


#ifdef WRITE_VID_DEBUG
void RpiControl::write_vid() {
    cv::VideoWriter writeVid("vid.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(1920, 1080));

    std::cout << "Writing video !" << std::endl;

    while(!camBufs_.empty()) {
        uint8_t *buf = camBufs_.front();
        camBufs_.pop();

        cv::Mat cvMat(1080, 1920, CV_8UC3, buf);
        writeVid.write(cvMat);
        free(buf);
    }

    std::cout << "Video vid.avi is ready !" << std::endl;

    writeVid.release();
}
#endif

void RpiControl::camFrameComplete(uint8_t *rpiBuf, size_t size) {
    int x = 0, y = 0;
    if (rpiBuf == nullptr)
        return;

#ifdef WRITE_VID_DEBUG
    uint8_t *buf = (uint8_t *)malloc(size * sizeof(uint8_t));
    memcpy(buf, rpiBuf, size);
    camBufs_.push(buf);
#endif

    std::cout << "Check for arrow !" << std::endl;
    rpiArrowDetect_.detectArrow(rpiBuf, size, x, y);
    std::cout << "Is there an arrow : x : " << x << ", y : " << y << std::endl;

    RpiMotorRover::MRPoint currentPoint;
    currentPoint.x = x;
    currentPoint.y = y;
    rpiRover_.addCurrentPoint(currentPoint);
}

void RpiControl::init() {
    rpiCamInst_.init();
    rpiCamInst_.rpiRequestComplete = std::bind(&RpiControl::camFrameComplete, this, std::placeholders::_1, std::placeholders::_2);
    rpiCamInst_.configure(FRAME_WIDTH, FRAME_HEIGHT, RpiCamera::RpiCameraFormat::RGB888);
    rpiCamInst_.allocBuffers();

    rpiArrowDetect_.setFramesRes(FRAME_WIDTH, FRAME_HEIGHT);

    RpiMotorRover::MRPoint targetPoint;
    targetPoint.x = FRAME_WIDTH / 2;
    targetPoint.y = FRAME_HEIGHT / 2;
    rpiRover_.init(targetPoint);

}

void RpiControl::start() {
    rpiCamInst_.start();
    rpiRover_.start();
}

int RpiControl::processRequest() {
    return rpiCamInst_.processRequest();
}

void RpiControl::stop() {
    rpiCamInst_.stop();
    rpiCamInst_.deinit();
    rpiRover_.stop();
    rpiRover_.deinit();

#ifdef WRITE_VID_DEBUG
    write_vid();
#endif
}