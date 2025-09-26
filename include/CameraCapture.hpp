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
    
    explicit CameraCapture(int camId) : capture_{camId}
    {
        if (!capture_.isOpened()) {
            throw std::runtime_error("[CameraCapture] - Failed to open camera Id: " + std::to_string(camId));
        }
    }
    ~CameraCapture() 
    {
        stop();
        capture_.release(); 
    }
    void start();
    void stop();
    std::shared_ptr<cv::Mat> getLatestFrame(); 

private:
    void frameCapture();
    cv::VideoCapture capture_;
    std::thread captureThread_;
    std::atomic<bool> isRunning_{false};
    std::shared_ptr<cv::Mat> latestFrame_;
    std::mutex frameMutex_; 
};

#endif