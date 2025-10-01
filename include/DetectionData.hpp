#ifndef DETECTIONDATA_HPP_
#define DETECTIONDATA_HPP_

#include <opencv2/core/types.hpp>

struct Detection
{
    int classId{};
    std::string className{};
    float confidence{};
    cv::Rect boundingBox{};

    Detection(const int id, const std::string& name, const float conf, const cv::Rect& bbox)
        :   classId(id), className(name), confidence(conf), boundingBox(bbox) {}
};

struct DetectionResult
{
    std::vector<Detection> detections;
};
#endif