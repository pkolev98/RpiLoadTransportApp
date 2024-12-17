#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <memory>

#include "rpi-service-callbacks.h"
#include "rpi-control.h"

class RpiService {
public:
    void init();

    void exec();

private:
    void start();
    void stop();

    serviceCallbacks callbacks_;
    std::unique_ptr<RpiControl> rpiControl_;
    std::atomic_bool bRunning_;
    std::mutex runningMutex_;
    std::condition_variable runningCV_;
};