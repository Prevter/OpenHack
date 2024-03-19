#pragma once

#include <random>
#include <imgui.h>
#include "platform/platform.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __clang__

#include <ctime>

#else

#include <time.h>

#endif

#include <chrono>

#if 0
#define L_BENCHMARK(name, code) do {\
    auto start = std::chrono::high_resolution_clock::now();\
    { code }\
    auto end = std::chrono::high_resolution_clock::now();  \
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();\
    L_TRACE("Benchmark {}: {}ms", name, time);\
} while (0)
#else
#define L_BENCHMARK(name, code) code
#endif

namespace openhack::utils {
    /// @brief Generates a random number between min and max.
    /// @param min The minimum value.
    /// @param max The maximum value.
    /// @return Random number between min and max.
    inline int random(int min, int max) {
        return min + (rand() % (max - min + 1));
    }

    /// @brief Generates a random number between min and max.
    /// @param min The minimum value.
    /// @param max The maximum value.
    /// @return Random number between min and max.
    inline float random(float min, float max) {
        return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
    }

    /// @brief Generates a random number between min and max.
    /// @param min The minimum value.
    /// @param max The maximum value.
    /// @return Random number between min and max.
    inline double random(double min, double max) {
        return min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (max - min)));
    }

    /// @brief Generates a random number between 0 and max.
    /// @param max The maximum value.
    /// @return Random number between 0 and max.
    inline int random(int max) {
        return random(0, max);
    }

    /// @brief Generates a random number between 0 and max.
    /// @param max The maximum value.
    /// @return Random number between 0 and max.
    inline float random(float max) {
        return random(0.f, max);
    }

    /// @brief Generates a random number between 0 and max.
    /// @param max The maximum value.
    /// @return Random number between 0 and max.
    inline double random(double max) {
        return random(0.0, max);
    }

    /// @brief Initializes the random number generator.
    inline void initRandom() {
        srand(time(nullptr));
    }

    /// @brief Get the time since the last frame.
    /// @return Time since the last frame in seconds.
    inline double getDeltaTime() {
        return ImGui::GetIO().DeltaTime;
    }

    /// @brief Convert degrees to radians.
    /// @param deg The degrees.
    /// @return The radians.
    inline double degToRad(double deg) {
        return deg * M_PI / 180.0;
    }

    /// @brief Rotate a vector by an angle.
    /// @param vector The vector to rotate.
    /// @param angle The angle in degrees.
    /// @return The rotated vector.
    inline ImVec2 rotateVector(const ImVec2 &vector, double angle) {
        auto rad = degToRad(angle);
        return {
                static_cast<float>(vector.x * cos(rad) - vector.y * sin(rad)),
                static_cast<float>(vector.x * sin(rad) + vector.y * cos(rad))
        };
    }

    /// @brief Converts a screen position to a frame position.
    /// @param pos The screen position.
    /// @return The frame position.
    inline ImVec2 screenToFrame(const ImVec2& pos) {
        auto *director = gd::cocos2d::CCDirector::sharedDirector();
        const auto frameSize = director->getOpenGLView()->getFrameSize();
        const auto winSize = director->getWinSize();

        return {
            pos.x / frameSize.width * winSize.width,
            (1.f - pos.y / frameSize.height) * winSize.height
        };
    }

    /// @brief Compare the version of the game with the given version.
    /// @param version The version to compare with, in the format "[>=|<=|>|<|=]x.yyy,x.yyy-x.yyy,..."
    /// @return True if the version is compatible with the given version.
    inline bool compareVersion(const std::string &version) {
        // Split the versions
        std::vector<std::string> parts;
        std::string versionCopy = version;
        std::string delimiter = ",";
        size_t pos = 0;
        std::string token;
        while ((pos = versionCopy.find(delimiter)) != std::string::npos) {
            token = versionCopy.substr(0, pos);
            parts.push_back(token);
            versionCopy.erase(0, pos + delimiter.length());
        }
        parts.push_back(versionCopy);

        // Get current version
        auto currentVersion = utils::getGameVersion();

        // Check if any of the versions are compatible
        bool match = false;
        for (auto &part: parts) {
            // Check if it's a range
            if (part.find('-') != std::string::npos) {
                std::vector<std::string> range;
                std::string rangeCopy = part;
                std::string rangeDelimiter = "-";
                size_t rangePos = 0;
                std::string rangeToken;
                while ((rangePos = rangeCopy.find(rangeDelimiter)) != std::string::npos) {
                    rangeToken = rangeCopy.substr(0, rangePos);
                    range.push_back(rangeToken);
                    rangeCopy.erase(0, rangePos + rangeDelimiter.length());
                }
                range.push_back(rangeCopy);

                if (range.size() != 2)
                    continue;

                if (strcmp(currentVersion.c_str(), range.at(0).c_str()) >= 0 &&
                    strcmp(currentVersion.c_str(), range.at(0).c_str()) <= 0) {
                    match = true;
                    break;
                }
            } else {
                // Get operator
                auto op = part.at(0);
                std::string ver = part.substr(1);
                std::string operation;
                switch (op) {
                    case '=':
                        operation = "==";
                        break;
                    case '!':
                        operation = "!=";
                        break;
                    case '>':
                        // Check if it's >=
                        if (part.at(1) == '=') {
                            operation = ">=";
                            ver = ver.substr(1);
                        } else {
                            operation = ">";
                        }
                        break;
                    case '<':
                        // Check if it's <=
                        if (part.at(1) == '=') {
                            operation = "<=";
                            ver = ver.substr(1);
                        } else {
                            operation = "<";
                        }
                        break;
                    default:
                        operation = "==";
                        break;
                }

                int32_t result = strcmp(currentVersion.c_str(), ver.c_str());
                if (operation == "==" && result == 0)
                    match = true;
                else if (operation == "!=" && result != 0)
                    match = true;
                else if (operation == "<" && result < 0)
                    match = true;
                else if (operation == "<=" && result <= 0)
                    match = true;
                else if (operation == ">" && result > 0)
                    match = true;
                else if (operation == ">=" && result >= 0)
                    match = true;

                if (match)
                    break;
            }
        }

        return match;
    }

    /// @brief Convert a string of hex characters to a vector of bytes.
    /// @param hex The string of hex characters.
    /// @return The vector of bytes.
    inline std::vector<uint8_t> hexToBytes(const std::string &hex) {
        // Check if contains "0x" prefix
        std::string hexCopy = hex;
        if (hexCopy.find("0x") != std::string::npos)
            hexCopy = hexCopy.substr(2);

        std::vector<uint8_t> bytes;
        size_t index = 0;
        while (index < hexCopy.size()) {
            // Skip spaces
            if (hexCopy.at(index) == ' ') {
                index++;
                continue;
            }

            // Convert from hex to byte
            std::string byteString = hexCopy.substr(index, 2);
            uint8_t byte = (uint8_t) strtol(byteString.c_str(), nullptr, 16);
            bytes.push_back(byte);
            index += 2;
        }

        return bytes;
    }

    /// @brief Convert a string of hex characters to an address.
    /// @param hex The string of hex characters.
    /// @return The address.
    inline uintptr_t hexToAddr(const std::string &hex) {
        // Check if contains "0x" prefix
        std::string hexCopy = hex;
        if (hexCopy.find("0x") != std::string::npos)
            hexCopy = hexCopy.substr(2);

        return (uintptr_t) strtol(hexCopy.c_str(), nullptr, 16);
    }

    /// @brief Convert a value to a vector of bytes.
    /// @tparam T The type of the value.
    /// @param value The value.
    /// @return The vector of bytes.
    template <typename T>
    inline std::vector<uint8_t> getBytes(T value) {
        return std::vector<uint8_t>((uint8_t *) &value, (uint8_t *) &value + sizeof(T));
    }

    /// @brief Convert a vector of bytes to a string of hex characters.
    /// @param bytes The vector of bytes.
    /// @return The string of hex characters.
    inline std::string bytesToHex(const std::vector<uint8_t> &bytes) {
        std::string hex;
        for (auto &byte: bytes) {
            char buf[3];
            sprintf(buf, "%02X", byte);
            hex += buf;
        }
        return hex;
    }

    /// @brief Check whether user is searching for something.
    /// @return True if search value is not empty
    inline bool isSearching() {
        return !config::getGlobal<std::string>("searchValue", "").empty();
    }

    /// @brief Check if the value is searched.
    /// @param value The value to check
    /// @return True if the value is searched
    inline bool isSearched(const std::string &value) {
        auto search = config::getGlobal<std::string>("searchValue", "");
        if (search.empty())
            return true;

        // Convert both to lower case
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        std::string lowerSearch = search;
        std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

        return lowerValue.find(lowerSearch) != std::string::npos;
    }

    /// @brief Get the current time in milliseconds.
    /// @return The current time in milliseconds.
    inline time_t getTime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
}