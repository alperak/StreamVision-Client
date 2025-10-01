#ifndef CAMERACAPTURE_HPP_
#define CAMERACAPTURE_HPP_

#include <opencv2/videoio.hpp>
#include <opencv2/core/mat.hpp>
#include <atomic>
#include <exception>
#include <iostream>
#include <thread>
#include <mutex>

class CameraCapture {
public:
    CameraCapture() = delete;
    CameraCapture (const CameraCapture&) = delete;
    CameraCapture& operator= (const CameraCapture&) = delete;
    CameraCapture(CameraCapture&&) = delete;
    CameraCapture& operator=(CameraCapture&&) = delete;
    
    explicit CameraCapture(int camId);
    ~CameraCapture();

    void start();
    void stop();

    cv::Mat getLatestFrame() const;

private:
    void frameCapture();

    cv::VideoCapture capture_;

    std::thread captureThread_;
    std::atomic<bool> isRunning_{false};

    cv::Mat latestFrame_;
    mutable std::mutex frameMutex_;
};

#endif