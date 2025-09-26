#ifndef FRAMEENCODER_HPP_
#define FRAMEENCODER_HPP_

#include <vector>
#include <opencv2/imgcodecs.hpp>

class FrameEncoder {
public:
    FrameEncoder() = delete;
    FrameEncoder(const FrameEncoder&) = delete;
    FrameEncoder& operator=(const FrameEncoder&) = delete;
    FrameEncoder(FrameEncoder&&) = delete;
    FrameEncoder& operator=(FrameEncoder&&) = delete;

    static std::vector<uchar> encodeJPEG(const cv::Mat& frame, int quality = defaultJPEGQuality_);
private:
    static constexpr int defaultJPEGQuality_ = 90;
};

#endif