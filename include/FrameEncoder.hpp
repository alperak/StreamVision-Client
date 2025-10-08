#ifndef FRAMEENCODER_HPP_
#define FRAMEENCODER_HPP_

#include <opencv2/imgcodecs.hpp>

/**
 * @class FrameEncoder
 * @brief Static utility class for encoding frames to various image formats
 *
 * Provides stateless encoding functions for converting cv::Mat frames
 * to compressed image formats suitable for network transmission or storage.
 */
class FrameEncoder {
public:
    FrameEncoder() = delete;
    FrameEncoder(const FrameEncoder&) = delete;
    FrameEncoder& operator=(const FrameEncoder&) = delete;
    FrameEncoder(FrameEncoder&&) = delete;
    FrameEncoder& operator=(FrameEncoder&&) = delete;

    /**
     * @brief Encodes a frame to JPEG format
     * @param frame Input frame to encode
     * @param quality JPEG quality [1-100], higher is better quality
     * @return Encoded JPEG data as byte vector
     */
    static std::vector<uchar> encodeJPEG(const cv::Mat& frame, int quality = defaultJPEGQuality_);

private:
    static constexpr int defaultJPEGQuality_{90};   ///< Default JPEG quality setting
};

#endif