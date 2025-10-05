#include "PipelineController.hpp"

PipelineController::PipelineController() : camera_{1}, frameHandler_{}, drawer_{}, webStream_{}
{

}

PipelineController::~PipelineController()
{
    stop();
}

void PipelineController::start() 
{
    if (isRunning_) {
        return;
    }

    camera_.start();
    frameHandler_.start();
    drawer_.start();
    webStream_.start();

    isRunning_ = true;
    pipelineThread_ = std::thread(&PipelineController::process, this);
}

void PipelineController::stop() 
{
    isRunning_ = false;

    if (pipelineThread_.joinable()) {
        pipelineThread_.join();
    }

    webStream_.stop();
    drawer_.stop();
    frameHandler_.stop();
    camera_.stop();
}

void PipelineController::process() {
    while (isRunning_) {
        cv::Mat frame = camera_.getLatestFrame();
        std::vector<uchar> encodedFrame = FrameEncoder::encodeJPEG(frame);
        if (!encodedFrame.empty()) {
            frameHandler_.pushEncodedFrame(std::move(encodedFrame));
            std::string detections = frameHandler_.getLatestDetections();
            DetectionResult parsedJson = JsonParser::parse(detections);
            drawer_.PushFrameAndDetections(frame, std::move(parsedJson));
            cv::Mat drawnFrame = drawer_.getDrawnFrame();
            webStream_.pushFrame(drawnFrame);

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
