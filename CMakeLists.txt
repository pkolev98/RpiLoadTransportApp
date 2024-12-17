cmake_minimum_required(VERSION 3.16)
project(RpiLoadTransport VERSION 0.1.0 LANGUAGES C CXX)
find_package( OpenCV REQUIRED )
find_package(PkgConfig)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)

include_directories(${CMAKE_SOURCE_DIR} ${LIBCAMERA_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(.)
include_directories(include/)
include_directories(src/grpc-server-entry)

add_subdirectory(src/)
add_subdirectory(test/)