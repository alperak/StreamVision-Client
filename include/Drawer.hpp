#ifndef DRAWER_HPP_
#define DRAWER_HPP_

#include "DetectionData.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

/**
 * @class Drawer
 * @brief Draws detection bounding boxes and labels on frames asynchronously
 *
 * Processes frame and detection results in a separate thread, drawing bounding boxes
 * with class labels and confidence scores. Provides thread safe access to the annotated frame.
 */
class Drawer {
public:
    Drawer(const Drawer&) = delete;
    Drawer& operator= (const Drawer&) = delete;
    Drawer(Drawer&&) = delete;
    Drawer& operator=(Drawer&&) = delete;

    Drawer() = default;

    /**
     * @brief Destructor - stops drawing thread and releases resources
     */
    ~Drawer();

    /**
     * @brief Starts asynchronous drawing thread
     */
    void start();

    /**
     * @brief Stops drawing thread
     * @note Blocks until drawing thread terminates
     */
    void stop();

    /**
     * @brief Submits a frame and its detections for visualization
     * @param frame Input frame to annotate
     * @param detections Detection results to draw
     */
    void setFrameAndDetections(cv::Mat frame, DetectionResult detections);

    /**
     * @brief Retrieves the most recent annotated frame
     * @return Shallow copy (ref count) of the frame with drawn detections
     */
    cv::Mat getDrawnFrame() const;

private:
    /**
     * @brief Main drawing loop running in separate thread
     */
    void drawDetectionsOnFrame();

    std::thread drawerThread_;                          ///< Background drawing thread
    std::atomic<bool> isRunning_{false};                ///< Drawing thread state

    cv::Mat drawnFrame_;                                ///< Output frame with annotations
    cv::Mat latestFrame_;                               ///< Input frame
    DetectionResult latestDetection_;                   ///< Detection vector
    mutable std::mutex frameMutex_;                     ///< Protects frame and detection access

    inline static const cv::Scalar kBoxColor{0, 255, 0};///< Bounding box color (green)
    static constexpr int kBoxThickness = 2;             ///< Bounding box line thickness
    static constexpr double kFontScale = 0.5;           ///< Label font scale
    static constexpr int kFontThickness = 1;            ///< Label font thickness
};

#endif