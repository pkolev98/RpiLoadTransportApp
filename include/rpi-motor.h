#pragma once

#include <pigpio.h>
#include <pigpiod_if2.h>

#define DEFAULT_SPEED 80

class RpiMotor {
public:
    struct ControlPins {
        int gpioIn1;
        int gpioIn2;
        int enPWM;
        int pinType[3];
    };

    int init(struct ControlPins &pins, int pwmFreq, int& pigpiodFd_);

    int moveForward();

    int moveBackward();

    int setSpeed(unsigned int speed);

    int stop();
private:
    struct ControlPins cPins_;
    int pigpiodFd_;
};