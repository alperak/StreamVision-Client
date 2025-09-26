#ifndef FrameSender_HPP_
#define FrameSender_HPP_

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <zmq.hpp>
#include <opencv2/core/cvdef.h> // For uchar

class FrameSender {
public:
    explicit FrameSender() : context_{ioThreadCount_}, clientSocket_(context_, zmq::socket_type::req) 
    {
        // Exception Handling lazım.
        clientSocket_.connect("tcp://localhost:5555");
        std::cout << "Connection success to localhost:5555\n";
    }
    ~FrameSender()
    {
        stop();
    }
    void start();
    void stop();

    void pushEncodedFrame(const std::vector<uchar>&& encodedBuffer);
    std::string getLatestJsonMetadata();
private:
    void sendFrameAndReceiveJsonMetadata();

    zmq::context_t context_;
    // Specifies how many threads will perform asynchronous I/O operations in the context.
    static constexpr int ioThreadCount_{1};
    zmq::socket_t clientSocket_;
    
    std::thread senderThread_;
    std::atomic<bool> isRunning_{false};

    std::vector<uchar> latestBuffer_;
    std::mutex bufferMutex_;

    std::string latestJsonMetadata_;
    std::mutex jsonMutex_;
};

#endif