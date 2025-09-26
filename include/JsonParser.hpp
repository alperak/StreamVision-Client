#ifndef JSONPARSER_HPP_
#define JSONPARSER_HPP_

#include "FrameMetadata.hpp"
#include <nlohmann/json.hpp>

class JsonParser {
public:
    JsonParser() = default;
    JsonParser(const JsonParser&) = delete;
    JsonParser& operator=(const JsonParser&) = delete;
    JsonParser(JsonParser&&) = delete;
    JsonParser& operator=(JsonParser&&) = delete;

    static FrameMetadata parse(const std::string& jsonStr);
private:

};

#endif