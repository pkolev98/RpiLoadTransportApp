#pragma once

#include <pigpio.h>

#define DEFAULT_SPEED 80

class RpiMotor {
public:
    struct ControlPins {
        int gpioIn1;
        int gpioIn2;
        int enPWM;
        int pinType[3];
    };

    int init(struct ControlPins &pins, int pwmFreq);

    int moveForward();

    int moveBackward();

    int setSpeed(unsigned int speed);

    int stop();
private:
    struct ControlPins cPins_;
};