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

class PipelineController {
public:
    PipelineController(const PipelineController&) = delete;
    PipelineController& operator= (const PipelineController&) = delete;
    PipelineController(PipelineController&&) = delete;
    PipelineController& operator=(PipelineController&&) = delete;

    PipelineController();
    ~PipelineController();

    void start();
    void stop();

private:
    void process();
    
    CameraCapture camera_;
    FrameHandler frameHandler_;
    Drawer drawer_;
    WebStream webStream_;

    std::thread pipelineThread_;
    std::atomic<bool> isRunning_{false};
};

#endif