#include "FrameEncoder.hpp"

std::vector<uchar> FrameEncoder::encodeJPEG(const cv::Mat& frame, int quality) 
{
    std::vector<uchar> encodedFrameBuffer;
    if (!frame.empty() && cv::imencode(".jpg", frame, encodedFrameBuffer, {cv::IMWRITE_JPEG_QUALITY, quality})) {
        return encodedFrameBuffer;
    }
    return encodedFrameBuffer;
}