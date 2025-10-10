#ifndef FRAMEHANDLER_HPP_
#define FRAMEHANDLER_HPP_

#include "ConfigXML.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <zmq.hpp>
#include <opencv2/core/cvdef.h> // For uchar

/**
 * @class FrameHandler
 * @brief Handles asynchronous frame sending and detection receiving via ZeroMQ
 *
 * Sends encoded frames to a remote detection server and receives JSON formatted
 * detection results as a string in a request-reply pattern. Runs in a separate thread.
 *
 * @note Uses ZeroMQ REQ-REP pattern. Server address loaded from ConfigXML.
 */
class FrameHandler {
public:

    FrameHandler(const FrameHandler&) = delete;
    FrameHandler& operator=(const FrameHandler&) = delete;
    FrameHandler(FrameHandler&&) = delete;
    FrameHandler& operator=(FrameHandler&&) = delete;

    /**
     * @brief Constructor - initializes ZeroMQ context and connects to server
     * @note Server IP and port are loaded from ConfigXML
     */
    FrameHandler();

    /**
     * @brief Destructor - stops communication and releases resources
     */
    ~FrameHandler();

    /**
     * @brief Starts asynchronous frame transmission thread
     */
    void start();

    /**
     * @brief Stops transmission thread and closes socket
     * @note Blocks until sender thread terminates
     */
    void stop();

    /**
     * @brief Submits an encoded frame for transmission
     * @param encodedFrame JPEG encoded frame data
     */
    void pushEncodedFrame(std::vector<uchar>&& encodedFrame);

    /**
     * @brief Retrieves the most recent detection results
     * @return JSON formatted string containing detection data
     */
    std::string getLatestDetections() const;

private:
    /**
     * @brief Main communication loop running in separate thread
     */
    void sendEncodedFrameAndReceiveDetections();

    zmq::context_t context_;                ///< ZeroMQ context
    static constexpr int ioThreadCount_{1}; ///< I/O threads for ZeroMQ context
    zmq::socket_t clientSocket_;            ///< REQ socket for server communication

    std::thread senderThread_;              ///< Background transmission thread
    std::atomic<bool> isRunning_{false};    ///< Thread state flag

    std::vector<uchar> latestEncodedFrame_; ///< Frame buffer for transmission
    std::mutex encodedFrameMutex_;          ///< Protects frame buffer access

    std::string latestDetections_;          ///< Latest detection results (JSON formatted string)
    mutable std::mutex detectionsMutex_;    ///< Protects detection data access
};

#endif