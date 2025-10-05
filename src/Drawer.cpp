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
    cv::Mat frameToDraw;
    DetectionResult detections;
    while (isRunning_) {
        if (isFrameAndDetectionSet_) {
            {
                std::lock_guard<std::mutex> lock(frameMutex_);
                frameToDraw = latestFrame_.clone();
                detections = std::move(latestDetection_);
            }

            // Each incoming frame may or may not contain detections, which is normal.
            // If detections are present, draw them on the frame.
            // Otherwise, return the original frame directly without drawing.
            if (!detections.detections.empty()) {
                for (const auto& detection : detections.detections) {
                    {
                        cv::rectangle(frameToDraw, detection.boundingBox, kBoxColor, kBoxThickness);
                        cv::putText(frameToDraw,
                                    detection.className + " " + std::to_string(static_cast<int>(detection.confidence * 100)) + "%",
                                    cv::Point(detection.boundingBox.x, detection.boundingBox.y - 5),
                                    cv::FONT_HERSHEY_SIMPLEX, kFontScale, kBoxColor, kFontThickness);
                    }
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
