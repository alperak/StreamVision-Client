#ifndef WEBSTREAM_HPP_
#define WEBSTREAM_HPP_

#include "FrameEncoder.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include <httplib.h>

/**
 * @class WebStream
 * @brief Provides HTTP based MJPEG video streaming for web browser viewing
 *
 * Streams frames as Motion JPEG over HTTP, allowing real time video viewing
 * in web browsers. Runs in a separate thread.
 *
 * @note Streams on http://localhost:8080/stream at ~30 FPS
 */
class WebStream {
public:
    WebStream(const WebStream&) = delete;
    WebStream& operator= (const WebStream&) = delete;
    WebStream(WebStream&&) = delete;
    WebStream& operator=(WebStream&&) = delete;

    WebStream() = default;

    /**
     * @brief Destructor - stops server and releases resources
     */
    ~WebStream();

    /**
     * @brief Starts HTTP server and MJPEG streaming
     */
    void start();

    /**
     * @brief Stops HTTP server and streaming thread
     * @note Blocks until server thread terminates
     */
    void stop();

    /**
     * @brief Submits a frame for streaming
     * @param frame Input frame to stream
     */
    void setFrame(cv::Mat frame);

private:
    /**
     * @brief Main MJPEG streaming loop running in separate thread
     */
    void runMjpegStream();

    httplib::Server server_;                ///< HTTP server instance
    static constexpr int port_{8080};       ///< Server listening port

    cv::Mat latestFrame_;                   ///< Frame for streaming
    std::mutex frameMutex_;                 ///< Protects frame access

    std::thread webServerThread_;           ///< Background server thread
    std::atomic<bool> isRunning_{false};    ///< Thread state flag
};

#endif