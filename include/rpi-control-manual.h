#pragma once

#include <cstdint>

#include "rpi-control.h"

class RpiControlManual : public RpiControl {
public:
    RpiControlManual() = default;

    ~RpiControlManual() = default;

    void camFrameComplete(uint8_t *rpiBuf, size_t size) override;

    void moveForward() override;
    void moveLeft() override;
    void moveRight() override;
};