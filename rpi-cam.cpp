#include <sys/mman.h>

#include <memory>
#include <thread>
#include <iomanip>

#include "rpi-cam.h"

using namespace std::chrono_literals;

RpiCamera *RpiCamera::inst_ = nullptr;

void RpiCamera::requestComplete(Request *request) {
    RpiCamera *rpiCam = RpiCamera::GetInstance();
    rpiCam->rpiRequestCompleted(request);
}

void RpiCamera::rpiRequestCompleted(Request *request) {
    if (request->status() == Request::RequestCancelled)
        return;

    const std::map<const Stream *, FrameBuffer *> &buffers = request->buffers();
    for (auto &buf_map : buffers) {
        FrameBuffer *fbuf = buf_map.second;
        const FrameMetadata &metadata = fbuf->metadata();
        std::cout << "Request completed for seq : " << std::setw(6) << std::setfill('0') << metadata.sequence << std::endl;

        if (rpiRequestComplete != nullptr) {
            rpiRequestComplete((uint8_t *)mappedBuffers_[fbuf->planes()[0].fd.get()], (size_t)fbuf->planes()[0].length);
        }
    }

    request->reuse(Request::ReuseBuffers);
    freeReqs_.push(request);
}

int RpiCamera::init() {
    cm_ = std::make_unique<CameraManager>();
    cm_->start();

    for (auto const &camera : cm_->cameras())
        std::cout << camera->id() << std::endl;

    auto cameras = cm_->cameras();
    if (cameras.empty()) {
        std::cout << "No cameras were identified on the system."
                << std::endl;
        cm_->stop();
        return EXIT_FAILURE;
    }

    std::string cameraId = cameras[0]->id();

    camera_ = cm_->get(cameraId);

    camera_->acquire();

    return 0;
}

int RpiCamera::configure(uint32_t width, uint32_t height, RpiCameraFormat fmt) {
    std::string rpiCamFmt;

    switch (fmt) {
        case RpiCameraFormat::BGR888:
            rpiCamFmt = "BGR888";
            break;
        case RpiCameraFormat::RGB888:
            rpiCamFmt = "RGB888";
            break;
        case RpiCameraFormat::NV12:
            rpiCamFmt = "NV12";
            break;
        case RpiCameraFormat::NV21:
            rpiCamFmt = "NV21";
            break;
        default:
            std::cout << "Not known rpi camera format!" << std::endl;
            break;
    }
    
    config_ = camera_->generateConfiguration( { StreamRole::Viewfinder } );

    StreamConfiguration &streamConfig = config_->at(0);
    std::cout << "Default viewfinder configuration is: " << streamConfig.toString() << std::endl;

    streamConfig.size.width = width;
    streamConfig.size.height = height;

    StreamFormats formats = streamConfig.formats();

    for (auto &fmt : formats.pixelformats()) {
        std::cout << "Available fmt : " << fmt << std::endl;
        if (fmt.toString() == rpiCamFmt) {
            streamConfig.pixelFormat = fmt;
            std::cout << "Set format to : " << streamConfig.pixelFormat << std::endl;
        }
    }

    config_->validate();
    std::cout << "Validated viewfinder configuration is: " << streamConfig.toString() << std::endl;

    camera_->configure(config_.get());

    streamConfig_ = streamConfig;

    return 0;    
}

int RpiCamera::allocBuffers() {
    allocator_ = new FrameBufferAllocator(camera_);

    for (StreamConfiguration &cfg : *config_) {
        int ret = allocator_->allocate(cfg.stream());
        if (ret < 0) {
            std::cerr << "Can't allocate buffers" << std::endl;
            return -ENOMEM;
        }

        size_t allocated = allocator_->buffers(cfg.stream()).size();
        std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
        stream_ = cfg.stream();
    }

    const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator_->buffers(stream_);
    //std::vector<std::unique_ptr<Request>> requests;

    for (unsigned int i = 0; i < buffers.size(); ++i) {
        std::unique_ptr<Request> request = camera_->createRequest();
        if (!request)
        {
            std::cerr << "Can't create request" << std::endl;
            return -ENOMEM;
        }

        const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream_, buffer.get());
        if (ret < 0)
        {
            std::cerr << "Can't set buffer for request"
                << std::endl;
            return ret;
        }

        int appBufFd = buffer->planes()[0].fd.get();
        if (appBufFd != -1) {
            uint8_t *appBuf = (uint8_t *)mmap(NULL, buffer->planes()[0].length, PROT_READ | PROT_WRITE, MAP_SHARED, appBufFd, 0);
            if (appBuf != MAP_FAILED) {
                std::cout << "Buffer mapped!!" << std::endl;
                mappedBuffers_.emplace(std::pair<int, uint8_t *>(appBufFd, appBuf));
            }
        }

        freeReqs_.push(request.get());
        requests_.push_back(std::move(request));
    }
    
    return 0;
}

int RpiCamera::start() {
    //std::function<void(Request *)> func = requestComplete;
    camera_->requestCompleted.connect(requestComplete);
    camera_->start();
    
    return 0;
}

int RpiCamera::processRequest() {

    if(freeReqs_.empty()) {
        return -1;
    }

    Request *req = freeReqs_.front();
    camera_->queueRequest(req);
    freeReqs_.pop();

    return 0;    
}

void RpiCamera::stop() {
    camera_->stop();
}

void RpiCamera::deinit() {
    for (auto &buf : allocator_->buffers(stream_)) {
        munmap(mappedBuffers_[buf->planes()[0].fd.get()], buf->planes()[0].length);
    }

    allocator_->free(stream_);
    delete allocator_;
    camera_->release();
    camera_.reset();
    cm_->stop();
}