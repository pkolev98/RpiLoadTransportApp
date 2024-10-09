#include <sys/mman.h>

#include <memory>
#include <unordered_map>
#include <cstdint>
#include <thread>
#include <fstream>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <libcamera/libcamera.h>

using namespace libcamera;
using namespace std::chrono_literals;

static std::shared_ptr<Camera> camera;
static std::unordered_map<int, uint8_t *> app_bufs;
std::queue<Request *> free_reqs;
std::queue<uint8_t *> cam_bufs;

void write_vid() {
    cv::VideoWriter write_vid("vid.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(1920, 1080));

    std::cout << "Writing video !" << std::endl;

    while(!cam_bufs.empty()) {
        uint8_t *buf = cam_bufs.front();
        cam_bufs.pop();

        cv::Mat cv_mat(1080, 1920, CV_8UC3, buf);
        write_vid.write(cv_mat);
        free(buf);
    }

    std::cout << "Video vid.avi is ready !" << std::endl;

    write_vid.release();
}

static void requestComplete(Request *request)
{
    if (request->status() == Request::RequestCancelled)
        return;

    const std::map<const Stream *, FrameBuffer *> &buffers = request->buffers();
    for (auto &buf_map : buffers) {
        FrameBuffer *fbuf = buf_map.second;
        const FrameMetadata &metadata = fbuf->metadata();
        std::cout << "Request completed for seq : " << std::setw(6) << std::setfill('0') << metadata.sequence << std::endl;

        uint8_t *buf = (uint8_t *)malloc(fbuf->planes()[0].length);
        memcpy(buf, app_bufs[fbuf->planes()[0].fd.get()], fbuf->planes()[0].length);
        cam_bufs.push(buf);

        //cv::Mat cv_mat(1080, 1920, CV_8UC3, app_bufs[fbuf->planes()[0].fd.get()]);
        //if (cv_mat.empty()) {
        //    std::cout << "Mat is empty! Mat size : " << cv_mat.size << std::endl;
        //}

        //if (!write_vid.isOpened()) {
        //    std::cout << "Error opening video!" << std::endl;
        //}

        //write_vid.write(cv_mat);
    }

    request->reuse(Request::ReuseBuffers);
    free_reqs.push(request);
}

int main() {
    int err = 0;
    int cnt = 0;
    size_t bufs_len = 0;

    std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();
    cm->start();

    for (auto const &camera : cm->cameras())
        std::cout << camera->id() << std::endl;

    auto cameras = cm->cameras();
    if (cameras.empty()) {
        std::cout << "No cameras were identified on the system."
                << std::endl;
        cm->stop();
        return EXIT_FAILURE;
    }

    std::string cameraId = cameras[0]->id();

    auto camera = cm->get(cameraId);

    camera->acquire();

    std::unique_ptr<CameraConfiguration> config = camera->generateConfiguration( { StreamRole::Viewfinder } );

    StreamConfiguration &streamConfig = config->at(0);
    std::cout << "Default viewfinder configuration is: " << streamConfig.toString() << std::endl;

    streamConfig.size.width = 1920;
    streamConfig.size.height = 1080;
    //streamConfig.pixelFormat = pixelFormat::nv12

    StreamFormats formats = streamConfig.formats();

    for (auto &fmt : formats.pixelformats()) {
        std::cout << "Available fmt : " << fmt << std::endl;
        if (fmt.toString() == "BGR888") {
            streamConfig.pixelFormat = fmt;
            std::cout << "Set format to : " << streamConfig.pixelFormat << std::endl;
        }
    }

    config->validate();
    std::cout << "Validated viewfinder configuration is: " << streamConfig.toString() << std::endl;

    camera->configure(config.get());

    FrameBufferAllocator *allocator = new FrameBufferAllocator(camera);

    for (StreamConfiguration &cfg : *config) {
        int ret = allocator->allocate(cfg.stream());
        if (ret < 0) {
            std::cerr << "Can't allocate buffers" << std::endl;
            return -ENOMEM;
        }

        size_t allocated = allocator->buffers(cfg.stream()).size();
        std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
    }

    Stream *stream = streamConfig.stream();
    const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream);
    std::vector<std::unique_ptr<Request>> requests;

    for (unsigned int i = 0; i < buffers.size(); ++i) {
        std::unique_ptr<Request> request = camera->createRequest();
        if (!request)
        {
            std::cerr << "Can't create request" << std::endl;
            return -ENOMEM;
        }

        const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream, buffer.get());
        if (ret < 0)
        {
            std::cerr << "Can't set buffer for request"
                << std::endl;
            return ret;
        }

        int app_buf_fd = buffer->planes()[0].fd.get();
        if (app_buf_fd != -1) {
            uint8_t *app_buf = (uint8_t *)mmap(NULL, buffer->planes()[0].length, PROT_READ | PROT_WRITE, MAP_SHARED, app_buf_fd, 0);
            if (app_buf != MAP_FAILED) {
                std::cout << "Buffer mapped!!" << std::endl;
                app_bufs.emplace(std::pair<int, uint8_t *>(app_buf_fd, app_buf));
            }
        }

        requests.push_back(std::move(request));
    }

    camera->requestCompleted.connect(requestComplete);

    camera->start();
    for (std::unique_ptr<Request> &request : requests) {
        camera->queueRequest(request.get());
        std::cout << "AAA QUEUE REUEST!!!" << std::endl;
    }

    for (;;) {
        if (free_reqs.empty()) {
            continue;
        }

        int seq = free_reqs.front()->findBuffer(stream)->metadata().sequence;

        if (seq > 300) {
            break;
        }

        camera->queueRequest(free_reqs.front());
        free_reqs.pop();
    }

    std::this_thread::sleep_for(1000ms);

    write_vid();

    camera->stop();

    for (auto &buf : buffers) {
        munmap(app_bufs[buf->planes()[0].fd.get()], buf->planes()[0].length);
    }

    allocator->free(stream);
    delete allocator;
    camera->release();
    camera.reset();
    cm->stop();
    //write_vid.release();

    return err;
}