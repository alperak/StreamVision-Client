#include "CameraCapture.hpp"
#include <opencv2/opencv.hpp>

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
    std::cout << "Thread is being stopped.\n";
    isRunning_ = false;
    if (captureThread_.joinable()) {
        captureThread_.join();
    }
}

std::shared_ptr<cv::Mat> CameraCapture::getLatestFrame()
{
    std::lock_guard<std::mutex> lock(frameMutex_);
    return latestFrame_;
}

void CameraCapture::frameCapture()
{
    cv::Mat frame;
    while (isRunning_) {
        if (capture_.read(frame)) {
            // Keep lock only for the duration of the frame update
            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                latestFrame_ = std::make_shared<cv::Mat>(frame);
            }
            // cv::imshow("Camera", *latestFrame_);
            // if (cv::waitKey(10) == 27) break; // ESC exit
        }
    }
}

