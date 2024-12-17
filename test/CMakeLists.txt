set(TEST_SOURCES_MOTOR
    rpi-motor.cpp
)

set(TEST_SOURCES_MOTOR_ROVER
    rpi-motor-rover.cpp
)

add_executable (TestMotor ${TEST_SOURCES_MOTOR})
add_dependencies(TestMotor RpiLoadTransportLib)
target_link_libraries(TestMotor RpiLoadTransportLib)

add_executable (TestMotorRover ${TEST_SOURCES_MOTOR_ROVER})
add_dependencies(TestMotorRover RpiLoadTransportLib)
target_link_libraries(TestMotorRover RpiLoadTransportLib)