#include "Drawer.hpp"

Drawer::~Drawer()
{
    stop();
}

void Drawer::start()
{
    if (isRunning_.exchange(true))
        return;

    drawerThread_ = std::thread(&Drawer::drawDetectionsOnFrame, this);
}

void Drawer::stop()
{
    if (!isRunning_.exchange(false))
        return;

    frameReady_.notify_all();

    if (drawerThread_.joinable())
        drawerThread_.join();
}

void Drawer::setFrameAndDetections(cv::Mat frame, DetectionResult detections)
{
    if (!frame.empty()) {
        std::lock_guard<std::mutex> lock(frameMutex_);
        latestFrame_ = std::move(frame);
        latestDetection_ = std::move(detections);
        hasNewFrame_ = true;
    }
    frameReady_.notify_one();
}

cv::Mat Drawer::getDrawnFrame() const {
    std::lock_guard<std::mutex> lock(frameMutex_);
    return drawnFrame_;
}

void Drawer::drawDetectionsOnFrame()
{
    while (isRunning_) {
        cv::Mat frameToDraw;
        DetectionResult detections;
        {
            std::unique_lock<std::mutex> lock(frameMutex_);
            // Wait until new frame arrived
            frameReady_.wait(lock, [this] {
                return hasNewFrame_ || !isRunning_;
            });

            if (!isRunning_) break;

            frameToDraw = latestFrame_.clone();
            detections = std::move(latestDetection_);
             hasNewFrame_ = false;
        }

        // Each incoming frame may or may not contain detections, which is normal.
        // If detections are present, draw them on the frame.
        // Otherwise, return the original frame directly without drawing.
        if (!detections.detections.empty()) {
            for (const auto& detection : detections.detections) {
                cv::rectangle(frameToDraw, detection.boundingBox, kBoxColor, kBoxThickness);
                cv::putText(frameToDraw,
                            detection.className + " " + std::to_string(static_cast<int>(detection.confidence * 100)) + "%",
                            cv::Point(detection.boundingBox.x, detection.boundingBox.y - 5),
                            cv::FONT_HERSHEY_SIMPLEX, kFontScale, kBoxColor, kFontThickness);
            }
        }

        {
            std::lock_guard<std::mutex> lock(frameMutex_);
            drawnFrame_ = std::move(frameToDraw);
        }
    }
}
