#include "rpi-control-manual.h"
#include "rpi-log.h"

static const RpiMotorRover::MRPoint centeredPoint(FRAME_WIDTH / 2, FRAME_HEIGHT / 2);
static const RpiMotorRover::MRPoint leftPoint(1, 1);
static const RpiMotorRover::MRPoint rightPoint(FRAME_WIDTH, FRAME_HEIGHT);

void RpiControlManual::camFrameComplete(uint8_t *rpiBuf, size_t size) {
    int x = 0, y = 0;
    static int framesCnt = 0;
    if (rpiBuf == nullptr)
        return;

    std::cout << "Check for arrow !" << std::endl;
    rpiArrowDetect_->detectArrow(rpiBuf, size, x, y);
    RPI_LOG("RpiControl", ERROR, "TEST!!!!");
    std::cout << "Is there an arrow : x : " << x << ", y : " << y << std::endl;
}

void RpiControlManual::moveForward() {
    rpiRover_->addCurrentPoint(centeredPoint);
}

void RpiControlManual::moveLeft() {
    rpiRover_->addCurrentPoint(leftPoint);
}

void RpiControlManual::moveRight() {
    rpiRover_->addCurrentPoint(rightPoint);
}

void RpiControlManual::moveStop() {
    rpiRover_->motorsStop();
}