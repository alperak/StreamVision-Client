#ifndef PIPELINECONTROLLER_HPP_
#define PIPELINECONTROLLER_HPP_

#include "CameraCapture.hpp"
#include "FrameHandler.hpp"
#include "FrameEncoder.hpp"
#include "JsonParser.hpp"
#include "Drawer.hpp"

#include <thread>
#include <atomic>
#include <iostream>

class PipelineController {
public:
    PipelineController() : camera_{1}, frameHandler_{}, drawer_{} {}
    ~PipelineController()
    {
        stop();
    }

    void start();
    void stop();
private:
    void process();
    
    CameraCapture camera_;
    FrameHandler frameHandler_;
    Drawer drawer_;

    std::thread pipelineThread_;
    std::atomic<bool> isRunning_{false};
};

#endif