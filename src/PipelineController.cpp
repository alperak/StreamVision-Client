#include "PipelineController.hpp"

void PipelineController::start() 
{
    if (isRunning_) {
        return; // Prevent multiple threads
    }

    camera_.start();
    frameSender_.start();

    isRunning_ = true;
    pipelineThread_ = std::thread(&PipelineController::process, this);
}

void PipelineController::stop() 
{
    isRunning_ = false;

    if (pipelineThread_.joinable()) {
        pipelineThread_.join();
    }

    frameSender_.stop();
    camera_.stop();
}

void PipelineController::process() {
    while (isRunning_) {
        std::shared_ptr<cv::Mat> frame = camera_.getLatestFrame();
        if (frame && !frame->empty()) {
            auto encoded = FrameEncoder::encodeJPEG(*frame);
            if (!encoded.empty()) {
                frameSender_.pushEncodedFrame(std::move(encoded));
                auto parsedJson = JsonParser::parse(frameSender_.getLatestJsonMetadata());
                
                std::cout << "Detections (" << parsedJson.detections.size() << "):\n";
                for (const auto& d : parsedJson.detections) {
                    std::cout << " - Label: " << d.label
                                << ", Confidence: " << d.confidence
                                << ", BBox: (" << d.boundingBox.x << ", "
                                                << d.boundingBox.y << ", "
                                                << d.boundingBox.width << "x"
                                                << d.boundingBox.height << ")\n";
                }
            }
        }
    }
}
