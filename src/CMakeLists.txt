add_subdirectory(grpc-server-entry)
add_subdirectory(rpi-load-transport)

add_executable(RpiLoadTransportApp main.cpp)
add_dependencies(RpiLoadTransportApp RpiLoadTransportLib)
target_link_libraries(RpiLoadTransportApp RpiLoadTransportLib)