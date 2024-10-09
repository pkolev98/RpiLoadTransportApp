cmake_minimum_required(VERSION 3.16)
project(test VERSION 0.1.0 LANGUAGES C CXX)
find_package( OpenCV REQUIRED )
find_package(PkgConfig)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)
#pkg_check_modules(LIBEVENT REQUIRED IMPORTED_TARGET libevent_pthreads)

include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS})

set(OPENCV_VIDEOIO_DEBUG TRUE)

add_executable(test test.cpp)
target_link_libraries(test ${OpenCV_LIBS})
#target_link_libraries(simple-cam PkgConfig::LIBEVENT)
target_link_libraries(test PkgConfig::LIBCAMERA)