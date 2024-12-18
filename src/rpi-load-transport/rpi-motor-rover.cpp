#include <iostream>

#include "rpi-motor-rover.h"

int RpiMotorRover::addCurrentPoint(const MRPoint &point) {
    std::unique_lock<std::mutex> lock (pointQueueMutex_);

    pointsQueue_.push(point);
    dataPresent_.store(true);
    pointsQueueCV_.notify_one();

    return 0;
}

int RpiMotorRover::processThread() {
    std::unique_lock<std::mutex> lock(pointQueueMutex_);

    while (roverRunning_.load()) {
        if (!pointsQueue_.empty()) {
            struct Point currentPoint = pointsQueue_.front();
            std::cout << "Point x : " << currentPoint.x << " y : " << currentPoint.y << std::endl;
            std::cout << "Target point : x : " << targetPoint_.x << " y : " << targetPoint_.y <<std::endl;
            pointsQueue_.pop();
            dataPresent_.store(false);

            lock.unlock();

            if (!currentPoint.x && !currentPoint.y) {
                currentRoverState_ = MRState::MR_IDLE;
            }
            else if ((currentPoint.x < targetPoint_.x) && (currentPoint.x < thresholdPoint.xLeft)) {
                currentRoverState_ = MRState::MR_LEFT;
            }
            else if (((currentPoint.x < targetPoint_.x) && (currentPoint.x >= thresholdPoint.xLeft)) ||
                     ((currentPoint.x > targetPoint_.x) && (currentPoint.x <= thresholdPoint.xRight))) {
                currentRoverState_ = MRState::MR_FORWARD;
            }
            else if ( (currentPoint.x > targetPoint_.x) && (currentPoint.x > thresholdPoint.xRight)) {
                currentRoverState_ = MRState::MR_RIGHT;
            }

            lock.lock();
            continue;
        }

        lock.unlock();

        switch (currentRoverState_)
        {
            case MRState::MR_FORWARD:
                std::cout << "Moving forward!" << std::endl;
                moveForward();
                break;
            case MRState::MR_LEFT:
                std::cout << "Move Left" << std::endl;
                turnLeft();
                break;
            case MRState::MR_RIGHT:
                std::cout << "Move Right" << std::endl;
                turnRight();
                break;
            case MRState::MR_IDLE:
                std::cout << "Go Idle" << std::endl;
                motorsStop();
                break;
            default:
                break;
        }

        lock.lock();
        pointsQueueCV_.wait(lock, [this] () { return dataPresent_.load(); });
    }

    return 0;
}

int RpiMotorRover::init(MRPoint &setTargetPoint) {
    pigpiodFd_ = pigpio_start(NULL, NULL);
    if (pigpiodFd_ < 0) {
        std::cout << "pigpio start failed!" << std::endl;
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

    leftMotor_.init(controlPinsLeft, 1000, pigpiodFd_);
    rightMotor_.init(controlPinsRight, 1000, pigpiodFd_);

    leftMotor_.setSpeed(80);
    rightMotor_.setSpeed(80);


    targetPoint_ = setTargetPoint;
    thresholdPoint.xLeft = targetPoint_.x - 100;
    thresholdPoint.xRight = targetPoint_.x + 100;
    thresholdPoint.yUp = targetPoint_.y - 100;
    thresholdPoint.yDown = targetPoint_.y + 100;

    return 0;
}

int RpiMotorRover::deinit() {
    pigpio_stop(pigpiodFd_);
    return 0;
}

int RpiMotorRover::start() {
    roverRunning_.store(true);
    dataPresent_.store(false);
    currentRoverState_ = MRState::MR_IDLE;

    roverThread_ = std::thread(&RpiMotorRover::processThread, this);

    return 0;
}

int RpiMotorRover::stop() {
    std::cout << "Deinitializing Motor Rover" << std::endl;

    {
        std::unique_lock lock(pointQueueMutex_);
        roverRunning_.store(false);
        dataPresent_.store(true);
        pointsQueueCV_.notify_one();
    }

    roverThread_.join();

    motorsStop();
    currentRoverState_ = MR_IDLE;

    std::cout << "Motor Rover deinitialized" << std::endl;

    return 0;
}

int RpiMotorRover::motorsStop() {
    leftMotor_.stop();
    rightMotor_.stop();

    return 0;
}

int RpiMotorRover::turnLeft() {
    leftMotor_.moveBackward();
    rightMotor_.moveForward();

    return 0;
}

int RpiMotorRover::turnRight() {
    leftMotor_.moveForward();
    rightMotor_.moveBackward();

    return 0;
}

int RpiMotorRover::moveForward() {
    leftMotor_.moveForward();
    rightMotor_.moveForward();

    return 0;
}