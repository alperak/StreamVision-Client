#include "PipelineController.hpp"

PipelineController::PipelineController() : camera_{}, frameHandler_{}, drawer_{}, webStream_{}
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

    // Start all pipeline components
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

    // Stop components in reverse order to ensure clean shutdown
    webStream_.stop();
    drawer_.stop();
    frameHandler_.stop();
    camera_.stop();
}

void PipelineController::process() {
    while (isRunning_) {
        // Get frame from camera
        cv::Mat frame = camera_.getLatestFrame();
        // Encode frame as JPEG
        std::vector<uchar> encodedFrame = FrameEncoder::encodeJPEG(frame);
        if (!encodedFrame.empty()) {
            // Send frame and receive detections from server
            frameHandler_.pushEncodedFrame(std::move(encodedFrame));
            std::string detections = frameHandler_.getLatestDetections();
            // Parse JSON detection results
            DetectionResult parsedJson = JsonParser::parse(detections);
            // Draw detections on frame
            drawer_.PushFrameAndDetections(frame, std::move(parsedJson));
            cv::Mat drawnFrame = drawer_.getDrawnFrame();
            // Stream annotated frame to web
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
