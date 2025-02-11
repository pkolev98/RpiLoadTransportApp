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

grpc::Status LoadTransportControlServer::set_mode(grpc::ServerContext* context, const SetModeRequest* request, Reply* response)
{
    std::cout << "set mode reuqested!!" << std::endl;
    bool isManualMode = request->mode() == RLTMode::MANUAL_MODE ? true : false;
    setMode_(isManualMode);

    return grpc::Status::OK;
}

grpc::Status LoadTransportControlServer::change_movement_type(grpc::ServerContext* context, const MoveRequest* request, Reply* response)
{
    std::cout << "change movement type reuqested!!" << std::endl;

    RpiControlMovementTypes RpiType;

    switch (request->move_type()) {
        case RLTMoveType::MOVE_FORWARD:
            RpiType = RpiControlMovementTypes::RPI_MOVE_FORWARD;
            break;
        case RLTMoveType::MOVE_RIGHT:
            RpiType = RpiControlMovementTypes::RPI_MOVE_RIGHT;
            break;
        case RLTMoveType::MOVE_LEFT:
            RpiType = RpiControlMovementTypes::RPI_MOVE_LEFT;
            break;
        case RLTMoveType::MOVE_STOP:
            RpiType = RpiControlMovementTypes::RPI_MOVE_STOP;
            break;
    }

    changeMovementType_(RpiType);

    return grpc::Status::OK;
}

void LoadTransportControlServer::setServiceCallbacks(serviceCallbacks& callbacks)
{
    start_ = callbacks.start;
    stop_ = callbacks.stop;
    setMode_ = callbacks.setMode;
    changeMovementType_ = callbacks.changeMovementType;
}