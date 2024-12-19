#include "rpi-control-automatic.h"
#include "rpi-log.h"

void RpiControlAutomatic::camFrameComplete(uint8_t *rpiBuf, size_t size) {
    int x = 0, y = 0;
    static int framesCnt = 0;
    if (rpiBuf == nullptr)
        return;

    std::cout << "Check for arrow !" << std::endl;
    rpiArrowDetect_->detectArrow(rpiBuf, size, x, y);
    RPI_LOG("RpiControl", ERROR, "TEST!!!!");
    std::cout << "Is there an arrow : x : " << x << ", y : " << y << std::endl;

    RpiMotorRover::MRPoint currentPoint;
    currentPoint.x = x;
    currentPoint.y = y;

    if (currentPoint.x != 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(33));
    rpiRover_->addCurrentPoint(currentPoint); }
}