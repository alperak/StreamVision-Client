#include "FrameHandler.hpp"

FrameHandler::FrameHandler() : context_{ioThreadCount_}, clientSocket_(context_, zmq::socket_type::req) 
{
    clientSocket_.connect("tcp://localhost:5555");
    std::cout << "Connection success to localhost:5555\n";
}

FrameHandler::~FrameHandler()
{
    stop();
}

void FrameHandler::start()
{
    if (isRunning_) {
        return;
    }
    isRunning_ = true;
    senderThread_ = std::thread(&FrameHandler::sendEncodedFrameAndReceiveDetections, this);
}

void FrameHandler::stop()
{
    isRunning_ = false;
    clientSocket_.close();
    if (senderThread_.joinable()) {
        senderThread_.join();
    }
}

void FrameHandler::pushEncodedFrame(std::vector<uchar>&& encodedFrame)
{
    if (!encodedFrame.empty()) {
        std::lock_guard<std::mutex> lock(encodedFrameMutex_);
        latestEncodedFrame_ = std::move(encodedFrame);
    }
}

std::string FrameHandler::getLatestDetections() const
{
    std::lock_guard<std::mutex> lock(detectionsMutex_);
    return latestDetections_;
}

void FrameHandler::sendEncodedFrameAndReceiveDetections()
{
    std::vector<uchar> encodedFrameToSend;
    while (isRunning_) {
        {
            std::lock_guard<std::mutex> lock(encodedFrameMutex_);
            // Use swap() instead of move() to enable buffer reuse and avoid repeated allocations
            encodedFrameToSend.swap(latestEncodedFrame_);
        }

        if(!encodedFrameToSend.empty()) {
            auto isEncodedFrameSent = clientSocket_.send(zmq::buffer(encodedFrameToSend), zmq::send_flags::none);
            if (isEncodedFrameSent) {
                zmq::message_t receivedMsg;
                auto isDetectionReceived = clientSocket_.recv(receivedMsg, zmq::recv_flags::none);
                if (isDetectionReceived) {
                    std::string detectionsData(static_cast<char*>(receivedMsg.data()), receivedMsg.size());
                    // The server responds with JSON formatted string.
                    // If there are no detections, the server sends JSON string representing an
                    // object with an empty "detections" array like {"detections":[]}.
                    // When a JSON string is declared but not assigned any data, json.dump() may return "null".
                    // Therefore, we explicitly check for these cases before processing the data.
                    // Having an empty detections array isn't a problem because not every frame necessarily contains detections
                    // but we still need to handle other invalid or null cases.
                    if (!detectionsData.empty() && detectionsData != "null") {
                        {
                            std::lock_guard<std::mutex> lock(detectionsMutex_);
                            latestDetections_ = std::move(detectionsData);
                        }
                    }
                }
            }
        }
    }
}