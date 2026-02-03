#ifndef JSONPARSER_HPP_
#define JSONPARSER_HPP_

#include "DetectionData.hpp"

#include <nlohmann/json.hpp>
#include <string>

/**
 * @class JsonParser
 * @brief Static utility class for parsing JSON formatted detection results
 *
 * Converts JSON strings received from detection server into structured
 * DetectionResult objects. Validates JSON structure before parsing.
 */
class JsonParser {
public:
    JsonParser() = default;
    JsonParser(const JsonParser&) = delete;
    JsonParser& operator=(const JsonParser&) = delete;
    JsonParser(JsonParser&&) = delete;
    JsonParser& operator=(JsonParser&&) = delete;

    /**
     * @brief Parses JSON string into detection results
     * @param jsonStr JSON formatted detection data
     * @return DetectionResult containing parsed detections
     *
     * Expected JSON format:
     * @code
     * {
     *   "detections": [
     *     {
     *       "classId": 0,
     *       "className": "person",
     *       "confidence": 0.95,
     *       "boundingBox": {"x": 100, "y": 150, "width": 50, "height": 80}
     *     }
     *   ]
     * }
     * @endcode
     */
    inline static DetectionResult parse(const std::string& jsonString) {
        DetectionResult detectionResult;

        if (!jsonString.empty()) {
            auto json = nlohmann::json::parse(jsonString);

            if (json.contains("detections") && json["detections"].is_array()) {

                for (const auto& detectionJson : json["detections"]) {
                    // Validate all required fields before parsing
                    if (detectionJson.contains("classId") && detectionJson.contains("className") &&
                        detectionJson.contains("confidence") && detectionJson.contains("boundingBox") &&
                        detectionJson["boundingBox"].is_object() && detectionJson["boundingBox"].contains("x") &&
                        detectionJson["boundingBox"].contains("y") && detectionJson["boundingBox"].contains("width") &&
                        detectionJson["boundingBox"].contains("height")) {
                            // Parse and construct Detection object
                            detectionResult.detections.emplace_back(
                            detectionJson["classId"].get<int>(),
                            detectionJson["className"].get<std::string>(),
                            detectionJson["confidence"].get<float>(),
                            cv::Rect(
                                detectionJson["boundingBox"]["x"].get<int>(),
                                detectionJson["boundingBox"]["y"].get<int>(),
                                detectionJson["boundingBox"]["width"].get<int>(),
                                detectionJson["boundingBox"]["height"].get<int>()
                            )
                        );
                    }
                }
            }
        }
        return detectionResult;
    }
};

#endif