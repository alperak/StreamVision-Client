#include "CameraCapture.hpp"

#include <spdlog/spdlog.h>

CameraCapture::CameraCapture()
{
    // Load camera settings from config
    const auto& config = ConfigXML::getInstance();

    const int camId = config.getCameraIndex();
    const int width = config.getCameraWidth();
    const int height = config.getCameraHeight();
    const int fps = config.getCameraFPS();

    // Open camera
    capture_.open(camId);

    if (!capture_.isOpened()) {
        throw std::runtime_error("[CameraCapture] - Failed to open camera Id: " + std::to_string(camId));
    }

    // Set camera properties
    capture_.set(cv::CAP_PROP_FRAME_WIDTH, width);
    capture_.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    capture_.set(cv::CAP_PROP_FPS, fps);

    // Get actual values
    const int actualWidth = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    const int actualHeight = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    const int actualFps = static_cast<int>(capture_.get(cv::CAP_PROP_FPS));

    spdlog::info("[CameraCapture] - Camera initialized successfully");
    spdlog::info("  Device Index : {}", camId);
    spdlog::info("  Requested    : {}x{} @ {} fps", width, height, fps);
    spdlog::info("  Actual       : {}x{} @ {} fps", actualWidth, actualHeight, actualFps);
}

CameraCapture::~CameraCapture()
{
    stop();
}

void CameraCapture::start()
{
    if (isRunning_.exchange(true))
        return;

    captureThread_ = std::thread(&CameraCapture::frameCapture, this);
}

void CameraCapture::stop()
{
    if (!isRunning_.exchange(false))
        return;

    if (captureThread_.joinable())
        captureThread_.join();

    capture_.release();
}

cv::Mat CameraCapture::getLatestFrame() const
{
    std::lock_guard<std::mutex> lock(frameMutex_);
    return latestFrame_;
}

void CameraCapture::frameCapture()
{
    while (isRunning_) {
        // Capture frame
        cv::Mat frame;
        if (capture_.read(frame)) {
            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                latestFrame_ = std::move(frame);
            }
        }
    }
}

