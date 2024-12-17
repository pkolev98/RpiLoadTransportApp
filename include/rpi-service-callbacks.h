#pragma once

#include <functional>

struct serviceCallbacks {
    std::function<void(void)> start;
    std::function<void(void)> stop;
};