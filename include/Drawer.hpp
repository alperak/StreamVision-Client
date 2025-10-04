#ifndef DRAWER_HPP_
#define DRAWER_HPP_

#include "DetectionData.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

class Drawer {
public:
    Drawer(const Drawer&) = delete;
    Drawer& operator= (const Drawer&) = delete;
    Drawer(Drawer&&) = delete;
    Drawer& operator=(Drawer&&) = delete;

    Drawer() = default;
    ~Drawer();
    void start();
    void stop();

    void PushFrameAndDetections(const cv::Mat& frame, DetectionResult&& detections);
    cv::Mat getDrawnFrame() const;

private:
    void drawDetectionsOnFrame();

    std::thread drawerThread_;
    std::atomic<bool> isRunning_{false};

    cv::Mat drawnFrame_;
    cv::Mat latestFrame_;
    DetectionResult latestDetection_;
    mutable std::mutex frameMutex_;

    std::atomic<bool> isFrameAndDetectionSet_{false};

    inline static const cv::Scalar kBoxColor{0, 255, 0}; // green
    static constexpr int kBoxThickness = 2;
    static constexpr double kFontScale = 0.5;
    static constexpr int kFontThickness = 1;
};

#endif