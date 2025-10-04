#ifndef WEBSTREAM_HPP_
#define WEBSTREAM_HPP_

#include "FrameEncoder.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include <httplib.h>

class WebStream {
public:
    WebStream(const WebStream&) = delete;
    WebStream& operator= (const WebStream&) = delete;
    WebStream(WebStream&&) = delete;
    WebStream& operator=(WebStream&&) = delete;

    WebStream() = default;
    ~WebStream();

    void start();
    void stop();

    void pushFrame(const cv::Mat& frame);

private:
    void runMjpegStream();

    httplib::Server server_;
    static constexpr int port_{8080};

    cv::Mat latestFrame_;
    std::mutex frameMutex_;

    std::thread webServerThread_;
    std::atomic<bool> isRunning_{false};
};

#endif