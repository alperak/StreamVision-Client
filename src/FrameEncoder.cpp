#include "FrameEncoder.hpp"

std::vector<uchar> FrameEncoder::encodeJPEG(const cv::Mat& frame, int quality) 
{
    std::vector<uchar> buffer;
    if (!frame.empty() && cv::imencode(".jpg", frame, buffer, {cv::IMWRITE_JPEG_QUALITY, quality})) {
        return buffer;
    }
    return {};
}