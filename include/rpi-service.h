#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <memory>

#include "rpi-service-callbacks.h"
#include "rpi-control.h"

class RpiService {
public:
    enum class ServiceState {
        RLT_SERVICE_IDLE = 0,
        RLT_SERVICE_MODE_SET,
        RLT_SERVICE_RUNNING
    };

    void init();

    void exec();

private:
    void onStart();
    void onStop();
    void onSetMode(bool isManualMode);

    serviceCallbacks callbacks_;
    std::unique_ptr<RpiControl> rpiControl_;
    std::atomic_bool bRunning_;
    std::mutex runningMutex_;
    std::condition_variable runningCV_;
    ServiceState serviceState_;
    bool isManualMode_{false};
    bool isControlModeSet{false};
};