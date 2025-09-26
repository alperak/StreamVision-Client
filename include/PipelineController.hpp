#ifndef PIPELINECONTROLLER_HPP_
#define PIPELINECONTROLLER_HPP_

#include "CameraCapture.hpp"
#include "FrameSender.hpp"
#include "FrameEncoder.hpp"
#include "JsonParser.hpp"

#include <thread>
#include <atomic>
#include <iostream>

class PipelineController {
public:
    PipelineController() : camera_{1}, frameSender_{} {}
    ~PipelineController()
    {
        stop();
    }

    void start();
    void stop();
private:
    void process();
    
    CameraCapture camera_;
    FrameSender frameSender_;

    std::thread pipelineThread_;
    std::atomic<bool> isRunning_{false};
};

#endif