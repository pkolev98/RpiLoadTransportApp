#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>

#include "rpi-service.h"
#include "load-transport-grpc-server.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

void RpiService::init()
{
    callbacks_.start = std::bind(&RpiService::start, this);
    callbacks_.stop = std::bind(&RpiService::stop, this);

    std::string server_address = "0.0.0.0:50051";
    LoadTransportControlServer service;

    service.setServiceCallbacks(callbacks_);

    //grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

void RpiService::start()
{
    std::unique_lock<std::mutex> lock(runningMutex_);
    rpiControl_ = std::make_unique<RpiControl>();
    rpiControl_->init();
    rpiControl_->start();
    bRunning_.store(true);
    runningCV_.notify_one();
}

void RpiService::stop()
{
    std::unique_lock<std::mutex> lock(runningMutex_);
    rpiControl_->stop();
    rpiControl_ = nullptr;
    bRunning_.store(false);
    runningCV_.notify_one();
}

void RpiService::exec()
{
    while(1) {
        {
            std::unique_lock<std::mutex> lock(runningMutex_);
            runningCV_.wait(lock, [this]( ) { return bRunning_.load(); });
        }

        if (rpiControl_)
            rpiControl_->processRequest();
    }
}