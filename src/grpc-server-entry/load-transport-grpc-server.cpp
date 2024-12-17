#include <iostream>

#include "load-transport-grpc-server.h"

grpc::Status LoadTransportControlServer::start(grpc::ServerContext* context, const StartRequest* request, Reply* response)
{
    std::cout << "Start reuqested!!" << std::endl;
    start_();

    return grpc::Status::OK;
}

grpc::Status LoadTransportControlServer::stop(grpc::ServerContext* context, const StopRequest* request, Reply* response)
{
    std::cout << "Stop reuqested!!" << std::endl;
    stop_();

    return grpc::Status::OK;
}

void LoadTransportControlServer::setServiceCallbacks(serviceCallbacks& callbacks)
{
    start_ = callbacks.start;
    stop_ = callbacks.stop;
}