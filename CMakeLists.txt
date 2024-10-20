cmake_minimum_required(VERSION 3.16)
project(test VERSION 0.1.0 LANGUAGES C CXX)
find_package( OpenCV REQUIRED )
find_package(PkgConfig)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)

include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(.)

if(WRITE_VID_DEBUG)
    add_compile_options("-DWRITE_VID_DEBUG=1")
endif()

set(OPENCV_VIDEOIO_DEBUG TRUE)

add_executable(ArrowDetect main.cpp rpi-cam.cpp rpi-arrow-detect.cpp rpi-control.cpp)
target_link_libraries(ArrowDetect ${OpenCV_LIBS})
target_link_libraries(ArrowDetect PkgConfig::LIBCAMERA)