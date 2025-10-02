#include "CameraCapture.hpp"

CameraCapture::CameraCapture(int camId) : capture_{camId}
{
    if (!capture_.isOpened()) {
        throw std::runtime_error("[CameraCapture] - Failed to open camera Id: " + std::to_string(camId));
    }
}
CameraCapture::~CameraCapture()
{
    stop();
}

void CameraCapture::start()
{
    if (isRunning_) {
        return; // Prevent multiple threads
    }
    isRunning_ = true;
    captureThread_ = std::thread(&CameraCapture::frameCapture, this);
}

void CameraCapture::stop()
{
    isRunning_ = false;
    if (captureThread_.joinable()) {
        captureThread_.join();
    }
}

cv::Mat CameraCapture::getLatestFrame() const
{
    std::lock_guard<std::mutex> lock(frameMutex_);
    return latestFrame_.clone();
}

void CameraCapture::frameCapture()
{
    cv::Mat frame;
    while (isRunning_) {
        if (capture_.read(frame)) {
            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                latestFrame_ = frame;
            }
        }
    }
}

