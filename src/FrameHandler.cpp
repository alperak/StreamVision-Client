#include "FrameHandler.hpp"

FrameHandler::FrameHandler() : context_{ioThreadCount_}, clientSocket_(context_, zmq::socket_type::req) 
{
    // Need exception handling
    clientSocket_.connect("tcp://localhost:5555");
    std::cout << "Connection success to localhost:5555\n";
}

FrameHandler::~FrameHandler()
{
    stop();
}

void FrameHandler::start()
{
    if (isRunning_) { // Prevent multiple threads
        return;
    }
    isRunning_ = true;
    senderThread_ = std::thread(&FrameHandler::sendEncodedFrameAndReceiveDetections, this);
}

void FrameHandler::stop()
{
    // May be necessary to close the socket
    isRunning_ = false;
    if (senderThread_.joinable()) {
        senderThread_.join();
    }
}

void FrameHandler::pushEncodedFrame(std::vector<uchar>&& encodedFrame)
{
    std::lock_guard<std::mutex> lock(encodedFrameMutex_);
    latestEncodedFrame_ = std::move(encodedFrame);
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

        // Need exception handling for send/receive
        if(!encodedFrameToSend.empty()) {
            auto isEncodedFrameSent = clientSocket_.send(zmq::buffer(encodedFrameToSend), zmq::send_flags::none);
            if (isEncodedFrameSent) {
                zmq::message_t receivedMsg;
                auto isDetectionReceived = clientSocket_.recv(receivedMsg, zmq::recv_flags::none);
                if (isDetectionReceived) {
                    std::string detectionsData(static_cast<char*>(receivedMsg.data()), receivedMsg.size());
                    // Server sends JSON responses. When json is declared but not populated,
                    // json.dump() returns the string "null", so we explicitly check for it
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