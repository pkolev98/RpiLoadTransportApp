#pragma once

#include <functional>

enum class RpiControlMovementTypes {
    RPI_MOVE_FORWARD = 0,
    RPI_MOVE_LEFT,
    RPI_MOVE_RIGHT,
    RPI_MOVE_STOP
};

struct serviceCallbacks {
    std::function<void(void)> start;
    std::function<void(void)> stop;
    std::function<void(bool isManualMode)> setMode;
    std::function<void(RpiControlMovementTypes)> changeMovementType;
};