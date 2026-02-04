#include "PipelineController.hpp"

#include <spdlog/spdlog.h>

PipelineController::PipelineController()
    : camera_(std::make_unique<CameraCapture>()),
      frameHandler_(std::make_unique<FrameHandler>()),
      drawer_(std::make_unique<Drawer>()),
      webStream_(std::make_unique<WebStream>()) {}

PipelineController::~PipelineController()
{
    stop();
}

void PipelineController::start() 
{
    if (isRunning_.exchange(true))
        return;

    // Start all pipeline components
    camera_->start();
    frameHandler_->start();
    drawer_->start();
    webStream_->start();

    pipelineThread_ = std::thread(&PipelineController::process, this);
    spdlog::info("[PipelineController] - Started");
}

void PipelineController::stop() 
{
    if (!isRunning_.exchange(false))
        return;

    if (pipelineThread_.joinable())
        pipelineThread_.join();

    // Reverse order shutdown
    webStream_->stop();
    drawer_->stop();
    frameHandler_->stop();
    camera_->stop();

    spdlog::info("[PipelineController] - Stopped");
}

void PipelineController::process() {
    while (isRunning_) {
        // Get frame from camera
        cv::Mat frame = camera_->getLatestFrame();
        if (!frame.empty()) {
            DetectionResult parsedJson;
            // Encode frame as JPEG
            std::optional<std::vector<uchar>> encodedFrame = FrameEncoder::encodeJPEG(frame);
            if (encodedFrame) {
                // Send frame and receive detections from server
                frameHandler_->setEncodedFrame(std::move(*encodedFrame));
                std::string detections = frameHandler_->getLatestDetections();
                // Parse JSON detection results
                parsedJson = JsonParser::parse(detections);
            }
            // Draw detections on frame
            drawer_->setFrameAndDetections(std::move(frame), std::move(parsedJson));
            cv::Mat drawnFrame = drawer_->getDrawnFrame();
            // Stream annotated frame to web
            webStream_->setFrame(drawnFrame);

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
