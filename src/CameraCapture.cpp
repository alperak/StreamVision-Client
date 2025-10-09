#include "CameraCapture.hpp"

CameraCapture::CameraCapture()
{
    // Load camera settings from config
    const auto& config = ConfigXML::getInstance();

    int camId = config.getCameraIndex();
    int width = config.getCameraWidth();
    int height = config.getCameraHeight();
    int fps = config.getCameraFPS();

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
    int actualWidth = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    int actualHeight = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    int actualFps = static_cast<int>(capture_.get(cv::CAP_PROP_FPS));

    std::cout << "[CameraCapture] - Camera initialized successfully\n"
              << "  Device Index : " << camId << '\n'
              << "  Requested    : " << width << "x" << height << " @ " << fps << " fps\n"
              << "  Actual       : " << actualWidth << "x" << actualHeight << " @ " << actualFps << " fps\n\n";
}

CameraCapture::~CameraCapture()
{
    stop();
}

void CameraCapture::start()
{
    if (isRunning_) {
        return;
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
    capture_.release();
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
        // Capture frame
        if (capture_.read(frame)) {
            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                latestFrame_ = frame;
            }
        }
    }
}

