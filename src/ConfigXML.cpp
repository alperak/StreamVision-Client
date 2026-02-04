#include "ConfigXML.hpp"

#include <spdlog/spdlog.h>

ConfigXML& ConfigXML::getInstance()
{
    static ConfigXML instance;
    return instance;
}

void ConfigXML::initialize()
{
    load();
    printCurrentConfig();
}

void ConfigXML::load()
{
    // Set defaults first  to ensures all values are valid
    setDefaults();

    // Check if config.xml exists
    if (!std::filesystem::exists(kconfigPath_)) {
        spdlog::warn("[ConfigXML] - File not found: {}. Creating with default values.", kconfigPath_);
        save();
        return;
    }

    // Try to load config.xml
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(kconfigPath_.c_str()) != tinyxml2::XML_SUCCESS) {
        spdlog::warn("[ConfigXML] - XML parsing error. Defaults will be used.");
        save();
        return;
    }

    // Verify root element exists
    tinyxml2::XMLElement* rootElem = doc.FirstChildElement("Config");
    if (!rootElem) {
        spdlog::warn("[ConfigXML] - Invalid XML format (root missing). Defaults will be used.");
        save();
        return;
    }

    // Load values from XML (overrides defaults where available)
    loadValues(rootElem);
    // Save to complete any missing elements
    save();
}

void ConfigXML::loadValues(tinyxml2::XMLElement* rootElem)
{
    // Load Camera settings
    tinyxml2::XMLElement* cameraElem = rootElem->FirstChildElement("Camera");
    if (cameraElem) {
        tinyxml2::XMLElement* idxElem = cameraElem->FirstChildElement("Index");
        if (idxElem && idxElem->GetText()) {
            cameraIndex_ = std::stoi(idxElem->GetText());
        }

        tinyxml2::XMLElement* widthElem = cameraElem->FirstChildElement("Width");
        if (widthElem && widthElem->GetText()) {
            cameraWidth_ = std::stoi(widthElem->GetText());
        }

        tinyxml2::XMLElement* heightElem = cameraElem->FirstChildElement("Height");
        if (heightElem && heightElem->GetText()) {
            cameraHeight_ = std::stoi(heightElem->GetText());
        }

        tinyxml2::XMLElement* fpsElem = cameraElem->FirstChildElement("FPS");
        if (fpsElem && fpsElem->GetText()) {
            cameraFPS_ = std::stoi(fpsElem->GetText());
        }
    }

    // Load Server settings
    tinyxml2::XMLElement* serverElem = rootElem->FirstChildElement("Server");
    if (serverElem) {
        tinyxml2::XMLElement* ipElem = serverElem->FirstChildElement("IP");
        if (ipElem && ipElem->GetText()) {
            serverIP_ = ipElem->GetText();
        }

        tinyxml2::XMLElement* portElem = serverElem->FirstChildElement("Port");
        if (portElem && portElem->GetText()) {
            serverPort_ = std::stoi(portElem->GetText());
        }
    }
}

void ConfigXML::save() const
{
    // Create config directory if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(kconfigPath_).parent_path());

    tinyxml2::XMLDocument doc;

    // Create Root element
    tinyxml2::XMLElement* rootElem = doc.NewElement("Config");
    doc.InsertFirstChild(rootElem);

    // Build Camera section
    tinyxml2::XMLElement* cameraElem = doc.NewElement("Camera");
    {
        tinyxml2::XMLElement* idxElem = doc.NewElement("Index");
        idxElem->SetText(cameraIndex_);
        cameraElem->InsertEndChild(idxElem);

        tinyxml2::XMLElement* widthElem = doc.NewElement("Width");
        widthElem->SetText(cameraWidth_);
        cameraElem->InsertEndChild(widthElem);

        tinyxml2::XMLElement* heightElem = doc.NewElement("Height");
        heightElem->SetText(cameraHeight_);
        cameraElem->InsertEndChild(heightElem);

        tinyxml2::XMLElement* fpsElem = doc.NewElement("FPS");
        fpsElem->SetText(cameraFPS_);
        cameraElem->InsertEndChild(fpsElem);
    }
    rootElem->InsertEndChild(cameraElem);

    // Build Server section
    tinyxml2::XMLElement* serverElem = doc.NewElement("Server");
    {
        tinyxml2::XMLElement* ipElem = doc.NewElement("IP");
        ipElem->SetText(serverIP_.c_str());
        serverElem->InsertEndChild(ipElem);
        tinyxml2::XMLElement* portElem = doc.NewElement("Port");
        portElem->SetText(serverPort_);
        serverElem->InsertEndChild(portElem);
    }
    rootElem->InsertEndChild(serverElem);

    // Write to file
    tinyxml2::XMLError result = doc.SaveFile(kconfigPath_.c_str());
    if (result != tinyxml2::XML_SUCCESS) {
        spdlog::error("[ConfigXML] - config.xml could not be saved (error code: {})", static_cast<int>(result));
    } else {
        spdlog::debug("[ConfigXML] - Settings saved: {}", kconfigPath_);
    }
}

void ConfigXML::setDefaults() noexcept
{
    cameraIndex_ = 0;
    cameraWidth_ = 640;
    cameraHeight_ = 480;
    cameraFPS_ = 30;
    serverIP_ = "0.0.0.0";
    serverPort_ = 5555;
}


void ConfigXML::printCurrentConfig() const noexcept
{
    spdlog::info("[ConfigXML] - Configuration loaded:");
    spdlog::info("  Camera: Index={}, {}x{} @ {}fps", cameraIndex_, cameraWidth_, cameraHeight_, cameraFPS_);
    spdlog::info("  Server: {}:{}", serverIP_, serverPort_);
}

int ConfigXML::getCameraIndex() const noexcept { return cameraIndex_; }
int ConfigXML::getCameraWidth() const noexcept { return cameraWidth_; }
int ConfigXML::getCameraHeight() const noexcept { return cameraHeight_; }
int ConfigXML::getCameraFPS() const noexcept { return cameraFPS_; }
std::string ConfigXML::getServerIP() const noexcept { return serverIP_; }
int ConfigXML::getServerPort() const noexcept { return serverPort_; }