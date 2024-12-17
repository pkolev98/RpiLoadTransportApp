#pragma once

#include <functional>

#include "control.grpc.pb.h"
#include "rpi-service-callbacks.h"

class LoadTransportControlServer : public LoadTransportControl::Service {
public:
    void setServiceCallbacks(serviceCallbacks& callbacks);

    grpc::Status start(grpc::ServerContext* context, const StartRequest* request, Reply* response) override;

    grpc::Status stop(grpc::ServerContext* context, const StopRequest* request, Reply* response) override;

private:
    std::function<void(void)> start_;
    std::function<void(void)> stop_;
};