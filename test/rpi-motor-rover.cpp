#include "rpi-motor-rover.h"
#include <thread>
#include <cstdlib>

int main() {
    RpiMotorRover rover;

    srand(time(NULL));
    int framecnt = 0;

    RpiMotorRover::Point target;
    target.x = 1280 / 2;
    //target.y = 720 / 2;

    rover.init(target);

    rover.start();

    for (uint32_t i = 0; i < 1280; i++)
    {
        target.x = i;
        rover.addCurrentPoint(target);
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    rover.stop();
    rover.deinit();

    return 0;
}