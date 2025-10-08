#ifndef DETECTIONDATA_HPP_
#define DETECTIONDATA_HPP_

#include <opencv2/core/types.hpp>

/**
 * @struct Detection
 * @brief Represents a single detected object
 */
struct Detection
{
    int classId{};              ///< Class identifier
    std::string className{};    ///< Readable class name
    float confidence{};         ///< Detection confidence score [0.0 - 1.0]
    cv::Rect boundingBox{};     ///< Bounding box coordinates (x, y, width, height)

    /**
     * @brief Constructs a detection object
     * @param id Class identifier
     * @param name Class name
     * @param conf Confidence score
     * @param bbox Bounding box rectangle
     */
    Detection(const int id, const std::string& name, const float conf, const cv::Rect& bbox)
        :   classId(id), className(name), confidence(conf), boundingBox(bbox) {}
};

/**
 * @struct DetectionResult
 * @brief Container for multiple detection results from a single frame
 */
struct DetectionResult
{
    std::vector<Detection> detections; ///< List of detected objects
};
#endif