#pragma once

#include <memory>
#include <unordered_map>
#include <cstdint>
#include <queue>
#include <cstdlib>

#include <libcamera/libcamera.h>

using namespace libcamera;

class RpiCamera {
public:
    enum class RpiCameraFormat {
        BGR888 = 0,
        RGB888,
        NV12,
        NV21
    };

    RpiCamera() {}

    void rpiRequestCompleted(Request *request);

    int init();

    int configure(uint32_t width, uint32_t height, RpiCameraFormat fmt);

    int allocBuffers();

    int start();

    int processRequest();

    void stop();

    void deinit();

    std::function<void(uint8_t *, size_t)> rpiRequestComplete;

private:

    std::unique_ptr<CameraManager> cm_;
    std::shared_ptr<Camera> camera_;
    std::unique_ptr<CameraConfiguration> config_;
    std::unordered_map<int, uint8_t *> mappedBuffers_;
    FrameBufferAllocator *allocator_;
    Stream *stream_;
    StreamConfiguration streamConfig_;
    std::vector<std::unique_ptr<Request>> requests_;
    std::queue<Request *> freeReqs_;
};