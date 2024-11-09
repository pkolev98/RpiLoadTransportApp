#include "rpi-motor.h"

int RpiMotor::init(struct ControlPins &pins, int pwmFreq) {
    cPins_ = pins;

    gpioSetMode(cPins_.gpioIn1, cPins_.pinType[0]);
    gpioSetMode(cPins_.gpioIn2, cPins_.pinType[1]);
    gpioSetMode(cPins_.enPWM, cPins_.pinType[2]);

    gpioSetPWMfrequency(cPins_.enPWM, pwmFreq);
    gpioPWM(cPins_.enPWM, DEFAULT_SPEED);
}

int RpiMotor::moveForward() {
    gpioWrite(cPins_.gpioIn1, PI_HIGH);
    gpioWrite(cPins_.gpioIn2, PI_LOW);
}

int RpiMotor::moveBackward() {
    gpioWrite(cPins_.gpioIn1, PI_LOW);
    gpioWrite(cPins_.gpioIn2, PI_HIGH);
}

int RpiMotor::setSpeed(unsigned int &speed) {
    if (speed > 255) {
        speed = 255;
    }

    gpioPWM(cPins_.enPWM, speed);
}

int RpiMotor::stop() {
    gpioWrite(cPins_.gpioIn1, 0);
    gpioWrite(cPins_.gpioIn2, 0);
    gpioPWM(cPins_.enPWM, 0);
}