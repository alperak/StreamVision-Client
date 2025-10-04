#include "Drawer.hpp"

Drawer::~Drawer()
{
    stop();
}

void Drawer::start()
{
    if (isRunning_) {
        return;
    }
    isRunning_ = true;
    drawerThread_ = std::thread(&Drawer::drawDetectionsOnFrame, this);
}

void Drawer::stop()
{
    isRunning_ = false;
    if (drawerThread_.joinable()) {
        drawerThread_.join();
    }
}

void Drawer::PushFrameAndDetections(const cv::Mat& frame, DetectionResult&& detections)
{
    if (!frame.empty()) {
        {
            std::lock_guard<std::mutex> lock(frameMutex_);
            latestFrame_ = frame.clone();
            latestDetection_ = std::move(detections);
        }
        isFrameAndDetectionSet_ = true;
    }
}

cv::Mat Drawer::getDrawnFrame() const
{
    std::lock_guard<std::mutex> lock(frameMutex_);
    return drawnFrame_.clone();
}

void Drawer::drawDetectionsOnFrame()
{
    while (isRunning_) {
        if (isFrameAndDetectionSet_) {
            cv::Mat frameToDraw;
            DetectionResult detections;
            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                frameToDraw = latestFrame_.clone();
                detections = std::move(latestDetection_);
            }

            for (const auto& detection : detections.detections) {
                {
                    cv::rectangle(frameToDraw, detection.boundingBox, kBoxColor, kBoxThickness);
                    cv::putText(frameToDraw,
                                detection.className + " " + std::to_string(int(detection.confidence * 100)) + "%",
                                cv::Point(detection.boundingBox.x, detection.boundingBox.y - 5),
                                cv::FONT_HERSHEY_SIMPLEX, kFontScale, kBoxColor, kFontThickness);
                }
            }

            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                drawnFrame_ = frameToDraw;
            }
            isFrameAndDetectionSet_ = false;
        }
    }
}
