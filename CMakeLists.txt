cmake_minimum_required(VERSION 3.16)
project(ArrowDetect VERSION 0.1.0 LANGUAGES C CXX)
find_package( OpenCV REQUIRED )
find_package(PkgConfig)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)

include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(.)
include_directories(include/)

set(SOURCES
    src/main.cpp
    src/rpi-arrow-detect.cpp
    src/rpi-cam.cpp
    src/rpi-control.cpp
    src/rpi-motor.cpp
    src/rpi-motor-rover.cpp
    )

if(WRITE_VID_DEBUG)
    add_compile_options("-DWRITE_VID_DEBUG=1")
endif()

set(OPENCV_VIDEOIO_DEBUG TRUE)

add_executable(ArrowDetect ${SOURCES})
target_link_libraries(ArrowDetect ${OpenCV_LIBS} PkgConfig::LIBCAMERA pigpio rt)
#target_link_libraries(ArrowDetect PkgConfig::LIBCAMERA)
#target_link_libraries(ArrowDetect pigpio)