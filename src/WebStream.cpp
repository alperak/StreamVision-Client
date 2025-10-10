#include "WebStream.hpp"

WebStream::~WebStream()
{
    stop();
}

void WebStream::start()
{
    if (isRunning_) {
        return;
    }
    isRunning_ = true;
    webServerThread_ = std::thread(&WebStream::runMjpegStream, this);
}

void WebStream::stop()
{
    isRunning_ = false;
    if (webServerThread_.joinable()) {
        webServerThread_.join();
    }
    server_.stop();
}

void WebStream::pushFrame(const cv::Mat& frame)
{
    if (!frame.empty()) {
        std::lock_guard<std::mutex> lock(frameMutex_);
        latestFrame_ = frame.clone();
    }
}

void WebStream::runMjpegStream()
{
    cv::Mat displayedFrame;
    // Register MJPEG endpoint
    server_.Get("/stream", [&](const httplib::Request&, httplib::Response &res) {
        // Set multipart MJPEG content type
        res.set_content_provider(
            "multipart/x-mixed-replace; boundary=frame",
            [&](size_t, httplib::DataSink &sink) {
                while (isRunning_) {
                    {
                        std::lock_guard<std::mutex> lock(frameMutex_);
                        displayedFrame = latestFrame_.clone();
                    }
                    auto encodedDisplayedFrame = FrameEncoder::encodeJPEG(displayedFrame);

                    std::string header = "--frame\r\n"
                                            "Content-Type: image/jpeg\r\n"
                                            "Content-Length: " + std::to_string(encodedDisplayedFrame.size()) + "\r\n\r\n";

                    sink.write(header.data(), header.size());
                    sink.write(reinterpret_cast<const char*>(encodedDisplayedFrame.data()), encodedDisplayedFrame.size());
                    sink.write("\r\n", 2);

                    // Limit the frame sending rate to ~30 FPS.
                    // Without this sleep, the loop runs as fast as possible, which can cause
                    // TCP buffers to fill up and the client to receive frames with noticeable delay,
                    // even if the latest frame is available. This ensures smooth and timely streaming.
                    std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30fps
                }
                return false; // stream end
            },
            nullptr // resource release
        );
    });
    server_.listen("localhost", port_);
}