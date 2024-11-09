#pragma once

#include <thread>
#include <pigpio.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "rpi-motor.h"

#define GPIO_17        17 // Raspberry GPIO 17
#define GPIO_27        27 // Raspberry GPIO 27
#define GPIO_18_PWM    18 // Raspberry GPIO 18
#define GPIO_22        22 // Raspberry GPIO 22
#define GPIO_23        23 // Raspberry GPIO 23
#define GPIO_19_PWM    19 // Raspberry GPIO 19

#define MOTOR_A_IN_1   GPIO_17
#define MOTOR_A_IN_2   GPIO_27
#define MOTOR_A_EN     GPIO_18_PWM
#define MOTOR_B_IN_1   GPIO_22
#define MOTOR_B_IN_2   GPIO_23
#define MOTOR_B_EN     GPIO_19_PWM

#define PWM_FREQUENCY 1000 // In Hz

class RpiMotorRover {
public:
    enum State {
        MR_FORWARD = 0,
        MR_LEFT,
        MR_RIGHT,
        MR_IDLE
    };

    struct Point {
        int x;
        int y;
    };

    struct TargetThreshold {
        int xLeft;
        int xRight;
        int yUp;
        int yDown;
    };

    using MRPoint = struct Point;
    using MRTargetThreshold = struct TargetThreshold;
    using MRState = enum State;

    int init(MRPoint &setTargetPoint);

    int start();

    int addCurrentPoint(const MRPoint &point);

    int stop();

    int deinit();
private:
    int processThread();

    int motorsStop();

    int turnLeft();

    int turnRight();

    int moveForward();

    std::thread roverThread_;
    MRState currentRoverState_;
    MRPoint targetPoint_;
    MRTargetThreshold thresholdPoint;
    RpiMotor leftMotor_;
    RpiMotor rightMotor_;
    std::queue<MRPoint> pointsQueue_;
    std::mutex pointQueueMutex_;
    std::condition_variable  pointsQueueCV_;
    std::atomic_bool dataPresent_;
    std::atomic_bool roverRunning_;
};