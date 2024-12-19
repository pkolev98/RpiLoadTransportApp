#pragma once

#include <cstdint>

#include "rpi-control.h"

class RpiControlAutomatic : public RpiControl {
public:
    void camFrameComplete(uint8_t *rpiBuf, size_t size) override;

    RpiControlAutomatic() = default;

    ~RpiControlAutomatic() = default;
};