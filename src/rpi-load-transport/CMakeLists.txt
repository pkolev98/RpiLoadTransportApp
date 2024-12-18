set(RLT_SOURCES
    rpi-service.cpp
    rpi-arrow-detect.cpp
    rpi-cam.cpp
    rpi-control.cpp
    rpi-motor.cpp
    rpi-motor-rover.cpp
    rpi-log.cpp
)

if(WRITE_VID_DEBUG)
    add_compile_options("-DWRITE_VID_DEBUG=1")
endif()

set(OPENCV_VIDEOIO_DEBUG TRUE)

add_library(RpiLoadTransportLib ${RLT_SOURCES})
add_dependencies(RpiLoadTransportLib LoadTransportControlLib)
target_link_libraries(RpiLoadTransportLib LoadTransportControlLib ${OpenCV_LIBS} PkgConfig::LIBCAMERA pigpiod_if2 rt pthread)