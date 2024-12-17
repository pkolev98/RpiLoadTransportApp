#include <thread>

#include "rpi-service.h"

int main() {
    RpiService service;
    std::thread createServer = std::thread( [&]() { service.init(); });
    service.exec();

    return 0;
}