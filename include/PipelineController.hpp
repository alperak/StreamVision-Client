#ifndef PIPELINECONTROLLER_HPP_
#define PIPELINECONTROLLER_HPP_

#include "CameraCapture.hpp"
#include "FrameHandler.hpp"
#include "FrameEncoder.hpp"
#include "JsonParser.hpp"
#include "Drawer.hpp"
#include "WebStream.hpp"
#include <thread>
#include <atomic>
#include <iostream>

/**
 * @class PipelineController
 * @brief Orchestrates the complete video processing pipeline
 *
 * Coordinates camera capture, frame encoding, remote detection, visualization,
 * and web streaming. Manages the complete lifecycle of all pipeline components.
 *
 * Pipeline flow:
 * Camera -> Encoder -> FrameHandler - Network (Detection) -> Parser -> Drawer -> Web Stream
 */
class PipelineController {
public:
    PipelineController(const PipelineController&) = delete;
    PipelineController& operator= (const PipelineController&) = delete;
    PipelineController(PipelineController&&) = delete;
    PipelineController& operator=(PipelineController&&) = delete;

    /**
     * @brief Constructor - initializes all pipeline components
     */
    PipelineController();

    /**
     * @brief Destructor - stops pipeline and releases resources
     */
    ~PipelineController();

    /**
     * @brief Starts all pipeline components and processing thread
     */
    void start();

    /**
     * @brief Stops all pipeline components and processing thread
     * @note Blocks until all components terminate gracefully
     */
    void stop();

private:
    /**
     * @brief Main processing loop coordinating all pipeline stages
     */
    void process();
    
    CameraCapture camera_;                  ///< Camera frame capture
    FrameHandler frameHandler_;             ///< Network communication with detection server
    Drawer drawer_;                         ///< Detection visualization
    WebStream webStream_;                   ///< HTTP streaming server

    std::thread pipelineThread_;            ///< Main processing thread
    std::atomic<bool> isRunning_{false};    ///< Thread state flag
};

#endif