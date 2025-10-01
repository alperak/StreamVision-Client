#ifndef FRAMEMETADATA_HPP_
#define FRAMEMETADATA_HPP_

#include <opencv2/core/types.hpp>
struct Detection {
    int classId{};
    std::string label{};
    float confidence{};
    cv::Rect boundingBox{};
};

struct FrameMetadata {
    std::vector<Detection> detections;
};
#endif