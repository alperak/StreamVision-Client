#ifndef CAMERACAPTURE_HPP_
#define CAMERACAPTURE_HPP_

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
    CameraCapture() = delete;
    CameraCapture (const CameraCapture&) = delete;
    CameraCapture& operator= (const CameraCapture&) = delete;
    CameraCapture(CameraCapture&&) = delete;
    CameraCapture& operator=(CameraCapture&&) = delete;
    
    /**
     * @brief Constructs and initializes camera capture
     * @param camId Camera device index (typically 0 for default camera)
     * @throws std::runtime_error if camera cannot be opened
     */
    explicit CameraCapture(int camId);

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