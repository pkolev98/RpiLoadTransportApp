#pragma once

#include <functional>

#include "control.grpc.pb.h"
#include "rpi-service-callbacks.h"

class LoadTransportControlServer : public LoadTransportControl::Service {
public:
    void setServiceCallbacks(serviceCallbacks& callbacks);

    grpc::Status start(grpc::ServerContext* context, const StartRequest* request, Reply* response) override;

    grpc::Status stop(grpc::ServerContext* context, const StopRequest* request, Reply* response) override;

    grpc::Status set_mode(grpc::ServerContext* context, const SetModeRequest* request, Reply* response) override;

    grpc::Status change_movement_type(grpc::ServerContext* context, const MoveRequest* request, Reply* response) override;

private:
    std::function<void(void)> start_;
    std::function<void(void)> stop_;
    std::function<void(bool isManualMode)> setMode_;
    std::function<void(RpiControlMovementTypes)> changeMovementType_;
};