#ifndef JSONPARSER_HPP_
#define JSONPARSER_HPP_

#include "DetectionData.hpp"
#include <nlohmann/json.hpp>

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
    static DetectionResult parse(const std::string& jsonStr);

private:

};

#endif