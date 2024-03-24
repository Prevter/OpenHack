#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <imgui.h>

/// @brief Namespace for all save data and configuration.
namespace openhack::config {
    inline nlohmann::json storage; // Object that holds options which are saved to disk
    inline nlohmann::json globals; // Object that holds temporary values

    /// @brief Check if a key exists in the configuration.
    /// @param key Key to check.
    /// @return True if the key exists in the configuration.
    inline bool has(const std::string &key) {
        return storage.contains(key);
    }

    /// @brief Get a value by key from the configuration.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @param defaultValue Default value to return if the key does not exist.
    /// @return Value from the configuration or the default value if the key does not exist.
    template<typename T>
    inline T get(const std::string &key, const T &defaultValue) {
        if (!has(key))
            return defaultValue;

        return storage.at(key).get<T>();
    }

    /// @brief Get a value by key from the configuration.
    /// @note If the key does not exist, it will throw an exception.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @return Value from the configuration.
    template<typename T>
    inline T get(const std::string &key) {
        if (!has(key))
            throw std::runtime_error("Key " + key + " does not exist");

        return storage.at(key).get<T>();
    }

    /// @brief Set a value by key in the configuration.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    inline void set(const std::string &key, const T &value) {
        storage[key] = value;
    }

    /// @brief Check if the value is of the specified type.
    /// @tparam T Type to check.
    /// @param key Key to check.
    /// @return True if the value is of the specified type.
    template<typename T>
    inline bool is(const std::string &key) {
        if (!has(key))
            return false;

        try {
            storage.at(key).get<T>();
            return true;
        } catch (const nlohmann::json::exception &e) {
            return false;
        }
    }

    /// @brief Set a value by key in the configuration if the key does not exist.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    inline void setIfEmpty(const std::string &key, const T &value) {
        if (!has(key))
            set(key, value);
    }

    /// @brief Check if a key exists in the global variables.
    /// @param key Key to check.
    /// @return True if the key exists in the global variables.
    inline bool hasGlobal(const std::string &key) {
        return globals.contains(key);
    }

    /// @brief Get a value by key from the global variables.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @param defaultValue Default value to return if the key does not exist.
    /// @return Value from the global variables or the default value if the key does not exist.
    template<typename T>
    inline T getGlobal(const std::string &key, const T &defaultValue) {
        if (!hasGlobal(key))
            return defaultValue;

        return globals.at(key).get<T>();
    }

    /// @brief Get a value by key from the global variables.
    /// @note If the key does not exist, it will throw an exception.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @return Value from the global variables.
    template<typename T>
    inline T getGlobal(const std::string &key) {
        if (!hasGlobal(key))
            throw std::runtime_error("Key " + key + " does not exist");

        return globals.at(key).get<T>();
    }

    /// @brief Set a value by key in the global variables.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    inline void setGlobal(const std::string &key, const T &value) {
        globals[key] = value;
    }

    /// @brief Set the default values for the configuration.
    void setDefaults();

    /// @brief Load the configuration from the file.
    void load();

    /// @brief Save the configuration to the file.
    void save();
}

/* == Extensions for `nlohmann::json` to support custom types. == */
#include "gui/animation/easing.hpp"
#include "gui/color.hpp"
#include "gui/themes/themes.hpp"
#include "gui/window.hpp"
#include "menu/blur.hpp"
#include "menu/keybinds.hpp"

void to_json(nlohmann::json &j, const ImVec2 &v);

void from_json(const nlohmann::json &j, ImVec2 &v);

namespace openhack::gui::animation {
    void to_json(nlohmann::json &j, const Easing &e);

    void from_json(const nlohmann::json &j, Easing &e);

    void to_json(nlohmann::json &j, const EasingMode &e);

    void from_json(const nlohmann::json &j, EasingMode &e);
}

namespace openhack::gui {
    void to_json(nlohmann::json &j, const Color &e);

    void from_json(const nlohmann::json &j, Color &e);

    void to_json(nlohmann::json &j, const Window &e);

    void from_json(const nlohmann::json &j, Window &e);

    void to_json(nlohmann::json &j, const std::vector<Window> &e);

    void from_json(const nlohmann::json &j, std::vector<Window> &e);
}

namespace openhack::menu::blur {
    void to_json(nlohmann::json &j, const State &e);

    void from_json(const nlohmann::json &j, State &e);
}

namespace openhack::menu::keybinds {
    void to_json(nlohmann::json &j, const Keybind &e);

    void from_json(const nlohmann::json &j, Keybind &e);

    void to_json(nlohmann::json &j, const std::vector<Keybind> &e);

    void from_json(const nlohmann::json &j, std::vector<Keybind> &e);
}