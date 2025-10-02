#include "PipelineController.hpp"

void PipelineController::start() 
{
    if (isRunning_) {
        return; // Prevent multiple threads
    }

    camera_.start();
    frameHandler_.start();

    isRunning_ = true;
    pipelineThread_ = std::thread(&PipelineController::process, this);
}

void PipelineController::stop() 
{
    isRunning_ = false;

    if (pipelineThread_.joinable()) {
        pipelineThread_.join();
    }

    frameHandler_.stop();
    camera_.stop();
}

void PipelineController::process() {
    while (isRunning_) {
        auto frame = camera_.getLatestFrame();
        if (!frame.empty()) {
            auto encodedFrame = FrameEncoder::encodeJPEG(frame);
            if (!encodedFrame.empty()) {
                frameHandler_.pushEncodedFrame(std::move(encodedFrame));
                auto detections = frameHandler_.getLatestDetections();
                auto parsedJson = JsonParser::parse(detections);

                /* std::cout << "Detections (" << parsedJson.detections.size() << "):\n";
                for (const auto& d : parsedJson.detections) {
                    std::cout << "classId: " << d.classId
                                << " , ClassName: " << d.className
                                << " , Confidence: " << d.confidence
                                << " , BBox: (" << d.boundingBox.x << ", "
                                                << d.boundingBox.y << ", "
                                                << d.boundingBox.width << ", "
                                                << d.boundingBox.height << ")\n";
                }*/
            }
        }
    }
}
