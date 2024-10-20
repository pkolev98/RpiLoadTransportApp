#include <sys/mman.h>

#include <memory>
#include <unordered_map>
#include <cstdint>
#include <thread>
#include <fstream>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "rpi-cam.h"
#include "rpi-arrow-detect.h"

using namespace std::chrono_literals;

std::queue<uint8_t *> camBufs;
RpiArrowDetect arrowDet;

void write_vid() {
    cv::VideoWriter writeVid("vid.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(1920, 1080));

    std::cout << "Writing video !" << std::endl;

    while(!camBufs.empty()) {
        uint8_t *buf = camBufs.front();
        camBufs.pop();

        cv::Mat cvMat(1080, 1920, CV_8UC3, buf);
        writeVid.write(cvMat);
        free(buf);
    }

    std::cout << "Video vid.avi is ready !" << std::endl;

    writeVid.release();
}

static void requestComplete(uint8_t *rpiBuf, size_t size)
{
    int x = 0, y = 0;
    if (rpiBuf == nullptr)
        return;

    //uint8_t *buf = (uint8_t *)malloc(size * sizeof(uint8_t));
    //1memcpy(buf, rpiBuf, size);
    std::cout << "Check for arrow !" << std::endl;
    arrowDet.detectArrow(rpiBuf, size, x, y);
    std::cout << "Is there an arrow : x : " << x << ", y : " << y << std::endl;
    //camBufs.push(buf);
}

int main() {
    int frame_cnt = 0;
    RpiCamera *rpiCam = RpiCamera::GetInstance();

    rpiCam->init();

    rpiCam->configure(1920, 1080, RpiCamera::RpiCameraFormat::BGR888);
    arrowDet.setFramesRes(1920, 1080);

    rpiCam->allocBuffers();

    rpiCam->rpiRequestComplete = &requestComplete;

    rpiCam->start();

    while(1) {
        if (rpiCam->processRequest() != -1)
            frame_cnt++;
    }

    write_vid();

    rpiCam->stop();
    rpiCam->deinit();

    return 0;
}