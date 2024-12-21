#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>

#include "rpi-service.h"
#include "load-transport-grpc-server.h"

#include "rpi-control-manual.h"
#include "rpi-control-automatic.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

void RpiService::init()
{
    callbacks_.start = std::bind(&RpiService::onStart, this);
    callbacks_.stop = std::bind(&RpiService::onStop, this);
    callbacks_.setMode = std::bind(&RpiService::onSetMode, this, std::placeholders::_1);
    serviceState_ = ServiceState::RLT_SERVICE_IDLE;

    std::string server_address = "0.0.0.0:50051";
    LoadTransportControlServer service;

    service.setServiceCallbacks(callbacks_);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    server->Wait();
}

void RpiService::onSetMode(bool isManualMode)
{
    std::unique_lock<std::mutex> lock(runningMutex_);
    if (serviceState_ == ServiceState::RLT_SERVICE_IDLE) {
        isManualMode_ = isManualMode;
        isControlModeSet = true;

        serviceState_ = ServiceState::RLT_SERVICE_MODE_SET;
    }
}

void RpiService::onStart()
{
    std::unique_lock<std::mutex> lock(runningMutex_);
    if ( isControlModeSet && serviceState_ != ServiceState::RLT_SERVICE_RUNNING ) {
        if ( isManualMode_ ) {
            rpiControl_ = std::make_unique<RpiControlManual>();
        } else {
            rpiControl_ = std::make_unique<RpiControlAutomatic>();
        }

        rpiControl_->init();
        rpiControl_->start();
        serviceState_ = ServiceState::RLT_SERVICE_RUNNING;

        bRunning_.store(true);
        runningCV_.notify_one();
    }
}

void RpiService::onStop()
{
    std::unique_lock<std::mutex> lock(runningMutex_);
    if (serviceState_ == ServiceState::RLT_SERVICE_RUNNING) {
        serviceState_ = ServiceState::RLT_SERVICE_IDLE;
        rpiControl_->stop();
        rpiControl_ = nullptr;
        bRunning_.store(false);
        runningCV_.notify_one();
    }
}

void RpiService::exec()
{
    while(1) {
        std::unique_lock<std::mutex> lock(runningMutex_);
        if (serviceState_ == ServiceState::RLT_SERVICE_RUNNING)
        {
            lock.unlock();
            rpiControl_->processRequest();
            lock.lock();
        }
        else
        {
            runningCV_.wait(lock, [this]( ) { return bRunning_.load(); });
        }
    }
}