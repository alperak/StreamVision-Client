#include "JsonParser.hpp"
#include <iostream>

FrameMetadata JsonParser::parse(const std::string& jsonString) 
{
    FrameMetadata metadata;

    try {
        auto json = nlohmann::json::parse(jsonString);

        if (json.contains("detections") && json["detections"].is_array()) {
            for (const auto& detectionJson : json["detections"]) {
                if (detectionJson.contains("label") && detectionJson.contains("confidence") &&
                    detectionJson.contains("boundingBox") && detectionJson["boundingBox"].is_object() &&
                    detectionJson["boundingBox"].contains("x") && detectionJson["boundingBox"].contains("y") &&
                    detectionJson["boundingBox"].contains("width") && detectionJson["boundingBox"].contains("height")) {
                
                    Detection detection;
                    detection.label = detectionJson["label"].get<std::string>();
                    detection.confidence = detectionJson["confidence"].get<float>();
                    detection.boundingBox = cv::Rect(detectionJson["boundingBox"]["x"].get<int>(),
                                                    detectionJson["boundingBox"]["y"].get<int>(),
                                                    detectionJson["boundingBox"]["width"].get<int>(),
                                                    detectionJson["boundingBox"]["height"].get<int>());
                    metadata.detections.push_back(detection);
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[JsonParser] - JSON parsing error: " << e.what() << "\n";
    }
    return metadata;
}