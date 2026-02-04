#include "FrameHandler.hpp"

#include <fstream>
#include <spdlog/spdlog.h>

FrameHandler::FrameHandler() : context_{kIoThreadCount_}, dealerSocket_(context_, zmq::socket_type::dealer) 
{
    // Get Identity from machine UniqueID for ROUTER (server) identification
    const std::string machineId = []() {
        std::ifstream file("/etc/machine-id");
        if (!file)
            throw std::runtime_error("[FrameHandler] - Failed to open /etc/machine-id");

        std::string id;
        std::getline(file, id);

        if (id.empty())
            throw std::runtime_error("[FrameHandler] - Machine ID is empty");

        return id;
    }();

    const std::string endpoint = "tcp://" + ConfigXML::getInstance().getServerIP() + ":" + std::to_string(ConfigXML::getInstance().getServerPort());

    try {
        // Set socket identity for ROUTER identification
        dealerSocket_.set(zmq::sockopt::routing_id, machineId);
        // Drop unsent on close
        dealerSocket_.set(zmq::sockopt::linger, 0);
        // 100ms recv timeout for responsive shutdown
        dealerSocket_.set(zmq::sockopt::rcvtimeo, 100);

        dealerSocket_.connect(endpoint);

    } catch (const zmq::error_t& e) {
        throw std::runtime_error("[FrameHandler] - Failed to connect to " + endpoint + ": " + e.what());
    }

    spdlog::info("[FrameHandler] - DEALER socket connected to {}", endpoint);
}

FrameHandler::~FrameHandler()
{
    stop();
}

void FrameHandler::start()
{
    if (isRunning_.exchange(true))
        return;

    spdlog::info("[FrameHandler] - Starting network I/O thread");
    frameHandler_ = std::thread(&FrameHandler::sendEncodedFrameAndReceiveDetections, this);
}

void FrameHandler::stop()
{
    if(!isRunning_.exchange(false))
        return;

    spdlog::info("[FrameHandler] - Stopping FrameHandler");

    context_.shutdown();

    if (frameHandler_.joinable())
        frameHandler_.join();

    spdlog::info("[FrameHandler] - Network thread joined");
}

void FrameHandler::setEncodedFrame(std::vector<uchar> encodedFrame)
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
    spdlog::debug("[FrameHandler] - Network I/O loop started");
    std::vector<uchar> encodedFrameToSend;

    while (isRunning_) {
        {
            std::lock_guard<std::mutex> lock(encodedFrameMutex_);
            encodedFrameToSend = std::move(latestEncodedFrame_);
        }

        if(!encodedFrameToSend.empty()) {
            try {
                /**
                 * DEALER-ROUTER Protocol:
                 *
                 * DEALER sends (2 frames):
                 *   Frame 0: empty delimiter
                 *   Frame 1: Encoded JPEG data
                 *
                 * ROUTER receives (3 frames):
                 *   Frame 0: identity (ROUTER adds automatically)
                 *   Frame 1: empty delimiter
                 *   Frame 2: Encoded JPEG data
                 *
                 * ROUTER sends (3 frames):
                 *   Frame 0: identity
                 *   Frame 1: empty delimiter
                 *   Frame 2: JSON data
                 *
                 * DEALER receives (2 frames because identity auto stripped):
                 *   Frame 0: empty delimiter
                 *   Frame 1: JSON data
                 */
                zmq::send_result_t isDelimiterSent = dealerSocket_.send(zmq::message_t{}, zmq::send_flags::sndmore);
                zmq::send_result_t isEncodedFrameSent = dealerSocket_.send(zmq::buffer(encodedFrameToSend), zmq::send_flags::none);

                if (isDelimiterSent && isEncodedFrameSent) {
                    zmq::message_t delimiter;
                    zmq::message_t receivedData;

                    // empty delimiter
                    zmq::recv_result_t isDelimiterReceived = dealerSocket_.recv(delimiter, zmq::recv_flags::none);
                    // actual JSON data
                    zmq::recv_result_t isDetectionReceived = dealerSocket_.recv(receivedData, zmq::recv_flags::none);

                    if (isDelimiterReceived && isDetectionReceived) {
                        std::string detectionsData(static_cast<char*>(receivedData.data()), receivedData.size());
                        /**
                         * The server responds with JSON formatted string.
                         * If there are no detections, the server sends JSON string representing an
                         * object with an empty "detections" array like {"detections":[]}.
                         * When a JSON string is declared but not assigned any data, json.dump() may return "null".
                         * Therefore, we explicitly check for these cases before processing the data.
                         * Having an empty detections array isn't a problem because not every frame necessarily contains detections
                         * but we still need to handle other invalid or null cases.
                         */
                        if (!detectionsData.empty() && detectionsData != "null") {
                            std::lock_guard<std::mutex> lock(detectionsMutex_);
                            latestDetections_ = std::move(detectionsData);
                        }
                    }
                }
            } catch (const zmq::error_t& e) {
                if (e.num() == ETERM) {
                    spdlog::debug("[FrameHandler] - Context terminated, exiting I/O loop");
                    break;
                }
                spdlog::error("[FrameHandler] - ZMQ error: {}", e.what());
            }
        }
        encodedFrameToSend.clear();
    }
    spdlog::debug("[FrameHandler] - Network I/O loop exited");
}