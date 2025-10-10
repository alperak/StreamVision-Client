#ifndef CONFIGXML_HPP_
#define CONFIGXML_HPP_

#include <string>
#include <filesystem>
#include <iostream>
#include <tinyxml2.h>

/**
 * @class ConfigXML
 * @brief Thread safe singleton class for managing application configuration from XML file
 *
 * This class provides centralized configuration management for camera and server settings.
 * It uses the Meyers Singleton pattern
 *
 * @note Initialize must be called from main thread before accessing config values
 *
 * Usage example:
 * @code
 * int main() {
 *     ConfigXML::getInstance().initialize();
 *     int cameraIdx = ConfigXML::getInstance().getCameraIndex();
 * }
 * @endcode
 */
class ConfigXML {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the singleton instance
     */
    static ConfigXML& getInstance();

    /**
     * @brief Initialize configuration from XML file
     *
     * Loads configuration from config.xml. If file doesn't exist or is invalid,
     * creates a new file with default values. Missing elements are automatically
     * filled with defaults.
     */
    void initialize();

    /**
     * @brief Get camera index
     * @return Camera device index (default: 0)
     */
    int getCameraIndex() const noexcept;

    /**
     * @brief Get camera resolution width
     * @return Camera width in pixels (default: 640)
     */
    int getCameraWidth() const noexcept;

    /**
     * @brief Get camera resolution height
     * @return Camera height in pixels (default: 480)
     */
    int getCameraHeight() const noexcept;

    /**
     * @brief Get camera frames per second
     * @return Camera FPS (default: 30)
     */
    int getCameraFPS() const noexcept;

    /**
     * @brief Get server IP address
     * @return Server IP as string (default: "0.0.0.0")
     */
    std::string getServerIP() const noexcept;

    /**
     * @brief Get server port number
     * @return Server port (default: 5555)
     */
    int getServerPort() const noexcept;

private:
    ConfigXML() = default;
    ~ConfigXML() = default;

    ConfigXML(const ConfigXML&) = delete;
    ConfigXML& operator=(const ConfigXML&) = delete;
    ConfigXML(ConfigXML&&) = delete;
    ConfigXML& operator=(ConfigXML&&) = delete;

    /**
     * @brief Load configuration from XML file
     *
     * Attempts to load config.xml. If file doesn't exist or parsing fails,
     * uses default values and creates a new file.
     */
    void load();

    /**
     * @brief Save current configuration to XML file
     *
     * Creates config directory if needed and writes all settings to XML.
     */
    void save() const;

    /**
     * @brief Set all configuration values to defaults
     */
    void setDefaults() noexcept;

    /**
     * @brief Parse and load values from XML root element
     * @param rootElem Pointer to XML root element
     *
     * Only overrides values that exist in XML. Missing values keep their defaults.
     */
    void loadValues(tinyxml2::XMLElement* rootElem);

    /**
     * @brief Print current configuration to console
     */
    void printCurrentConfig() const noexcept;

    int cameraIndex_{};     ///< Camera device index
    int cameraWidth_{};     ///< Camera resolution width
    int cameraHeight_{};    ///< Camera resolution height
    int cameraFPS_{};       ///< Camera frames per second

    std::string serverIP_{};    ///< Server IP address
    int serverPort_{};          ///< Server port number

    const std::string kconfigPath_{"../config/config.xml"}; ///< Config file path
};

#endif