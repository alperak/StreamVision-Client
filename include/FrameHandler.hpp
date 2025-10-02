#ifndef FRAMEHANDLER_HPP_
#define FRAMEHANDLER_HPP_

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <zmq.hpp>
#include <opencv2/core/cvdef.h> // For uchar

class FrameHandler {
public:

    FrameHandler(const FrameHandler&) = delete;
    FrameHandler& operator=(const FrameHandler&) = delete;
    FrameHandler(FrameHandler&&) = delete;
    FrameHandler& operator=(FrameHandler&&) = delete;

    FrameHandler();
    ~FrameHandler();

    void start();
    void stop();

    void pushEncodedFrame(std::vector<uchar>&& encodedFrame);
    std::string getLatestDetections() const;

private:
    void sendEncodedFrameAndReceiveDetections();

    zmq::context_t context_;
    // Specifies how many threads will perform asynchronous I/O operations in the context.
    static constexpr int ioThreadCount_{1};
    zmq::socket_t clientSocket_;

    std::thread senderThread_;
    std::atomic<bool> isRunning_{false};

    std::vector<uchar> latestEncodedFrame_;
    std::mutex encodedFrameMutex_;

    std::string latestDetections_;
    mutable std::mutex detectionsMutex_;
};

#endif