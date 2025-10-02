#include "JsonParser.hpp"
#include <iostream>

DetectionResult JsonParser::parse(const std::string& jsonString)
{
    DetectionResult detectionResult;

    if (!jsonString.empty()) {
        auto json = nlohmann::json::parse(jsonString);

        if (json.contains("detections") && json["detections"].is_array()) {

            for (const auto& detectionJson : json["detections"]) {

                if (detectionJson.contains("classId") && detectionJson.contains("className") &&
                    detectionJson.contains("confidence") && detectionJson.contains("boundingBox") &&
                    detectionJson["boundingBox"].is_object() && detectionJson["boundingBox"].contains("x") &&
                    detectionJson["boundingBox"].contains("y") && detectionJson["boundingBox"].contains("width") &&
                    detectionJson["boundingBox"].contains("height")) {

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