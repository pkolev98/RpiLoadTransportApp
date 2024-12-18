#include "rpi-motor.h"

int RpiMotor::init(struct ControlPins &pins, int pwmFreq, int& pigpiodFd) {
    cPins_ = pins;
    pigpiodFd_ = pigpiodFd;

    set_mode(pigpiodFd_, cPins_.gpioIn1, cPins_.pinType[0]);
    set_mode(pigpiodFd_, cPins_.gpioIn2, cPins_.pinType[1]);
    set_mode(pigpiodFd_, cPins_.enPWM, cPins_.pinType[2]);

    set_PWM_frequency(pigpiodFd_, cPins_.enPWM, pwmFreq);
    set_PWM_dutycycle(pigpiodFd_, cPins_.enPWM, DEFAULT_SPEED);
}

int RpiMotor::moveForward() {
    gpio_write(pigpiodFd_, cPins_.gpioIn1, PI_HIGH);
    gpio_write(pigpiodFd_, cPins_.gpioIn2, PI_LOW);
}

int RpiMotor::moveBackward() {
    gpio_write(pigpiodFd_, cPins_.gpioIn1, PI_LOW);
    gpio_write(pigpiodFd_, cPins_.gpioIn2, PI_HIGH);
}

int RpiMotor::setSpeed(unsigned int speed) {
    if (speed > 255) {
        speed = 255;
    }

    set_PWM_dutycycle(pigpiodFd_, cPins_.enPWM, speed);
}

int RpiMotor::stop() {
    gpio_write(pigpiodFd_, cPins_.gpioIn1, PI_LOW);
    gpio_write(pigpiodFd_, cPins_.gpioIn2, PI_LOW);
    set_PWM_dutycycle(pigpiodFd_, cPins_.enPWM, PI_LOW);
}