#ifndef CAMERACAPTURE_HPP_
#define CAMERACAPTURE_HPP_

#include "ConfigXML.hpp"
#include <opencv2/videoio.hpp>
#include <opencv2/core/mat.hpp>
#include <atomic>
#include <exception>
#include <iostream>
#include <thread>
#include <mutex>

/**
 * @class CameraCapture
 * @brief Manages asynchronous camera frame capture in a separate thread
 *
 * Provides thread safe access to the latest captured frame from a camera device.
 * Frame capture runs continuously in a background thread until stopped.
 */
class CameraCapture {
public:
    CameraCapture (const CameraCapture&) = delete;
    CameraCapture& operator= (const CameraCapture&) = delete;
    CameraCapture(CameraCapture&&) = delete;
    CameraCapture& operator=(CameraCapture&&) = delete;
    
    /**
     * @brief Constructs and initializes camera capture from config
     * @throws std::runtime_error if camera cannot be opened
     * @note Camera parameters (index, resolution, fps) are loaded from ConfigXML
     */
    CameraCapture();

    /**
     * @brief Destructor - stops capture and releases resources
     */
    ~CameraCapture();

    /**
     * @brief Starts asynchronous frame capture
     */
    void start();

    /**
     * @brief Stops frame capture and releases camera
     * @note Blocks until capture thread terminates
     */
    void stop();

    /**
     * @brief Retrieves the most recent captured frame
     * @return Deep copy of the latest frame
     */
    cv::Mat getLatestFrame() const;

private:
    /**
     * @brief Main capture loop running in separate thread
     */
    void frameCapture();

    cv::VideoCapture capture_;              ///< OpenCV camera interface

    std::thread captureThread_;             ///< Background capture thread
    std::atomic<bool> isRunning_{false};    ///< Capture state flag

    cv::Mat latestFrame_;                   ///< Most recent frame
    mutable std::mutex frameMutex_;         ///< Protects latestFrame_ access
};

#endif