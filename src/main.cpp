#include <sys/mman.h>

#include <memory>
#include <unordered_map>
#include <cstdint>
#include <thread>
#include <fstream>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "rpi-control.h"

using namespace std::chrono_literals;

int main() {
    int frame_cnt = 0;
    RpiControl rpiControl;

    rpiControl.init();

    rpiControl.start();

    while(1) {
        if (rpiControl.processRequest() != -1)
            frame_cnt++;

        if (frame_cnt == 300) {
            break;
        }
    }

    rpiControl.stop();

    return 0;
}