#include <iostream>
#include <unistd.h>

#include "rpi-motor-rover.h"

int main()
{
    RpiMotor leftMotor;
    RpiMotor rightMotor;

    if (gpioInitialise() < 0) {
        std::cout << "pigpio initialization failed!" << std::endl;
        return -1;
    }

    struct RpiMotor::ControlPins controlPinsLeft = {
                                                MOTOR_A_IN_1,
                                                MOTOR_A_IN_2,
                                                MOTOR_A_EN,
                                                { PI_OUTPUT, PI_OUTPUT, PI_OUTPUT }
                                               };

    struct RpiMotor::ControlPins controlPinsRight = {
                                                MOTOR_B_IN_1,
                                                MOTOR_B_IN_2,
                                                MOTOR_B_EN,
                                                { PI_OUTPUT, PI_OUTPUT, PI_OUTPUT }
                                               };

    leftMotor.init(controlPinsLeft, 1000);
    rightMotor.init(controlPinsRight, 1000);

    leftMotor.moveForward();
    rightMotor.moveForward();
    
    sleep(3);

    leftMotor.moveBackward();
    rightMotor.moveForward();

    sleep(3);

    rightMotor.moveBackward();
    leftMotor.moveForward();

    sleep(3);

    rightMotor.moveBackward();
    leftMotor.moveBackward();

    sleep(3);

    rightMotor.stop();
    leftMotor.stop();

    gpioTerminate();

    return 0;
}