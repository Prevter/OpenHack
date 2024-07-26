#pragma once

#include <utility>
#include <string>
#include <cstdint>
#include <functional>

/// @brief All logic related to keybindings.
namespace openhack::menu::keybinds {
    struct Keybind {
        std::string name;
        std::string id;

        uint32_t keycode;
        std::function<void()> callback;

        Keybind(std::string name, std::string id, uint32_t keycode)
                : name(std::move(name)), id(std::move(id)), keycode(keycode), callback(nullptr) {}

        Keybind(std::string name, std::string id, uint32_t keycode, const std::function<void()> &callback)
                : name(std::move(name)), id(std::move(id)), keycode(keycode), callback(callback) {}
    };

    /// @brief Adds a new keybind.
    /// @param keybind The keybind to add.
    void addKeybind(const Keybind &keybind);

    /// @brief Removes a keybind by its ID.
    /// @param id The ID of the keybind to remove.
    void removeKeybind(const std::string &id);

    /// @brief Gets a keybind by its ID.
    /// @param id The ID of the keybind to get.
    /// @return The keybind with the specified ID.
    Keybind getKeybind(const std::string &id);

    /// @brief Checks if a keybind with the specified ID exists.
    /// @param id The ID of the keybind to check.
    /// @return True if a keybind with the specified ID exists.
    bool hasKeybind(const std::string &id);

    /// @brief Gets all keybinds.
    /// @return All keybinds.
    std::vector<Keybind>& getKeybinds();

    /// @brief Sets the callback for a keybind if it exists.
    /// @param id The ID of the keybind.
    /// @param callback The callback to set.
    void setKeybindCallback(const std::string &id, const std::function<void()> &callback);

    /// @brief Loads the keybinds from the configuration file and creates the window.
    /// @warning It doesn't create keybinding callbacks, they need to be added by each hack separately.
    /// @note This has to be called before hacks are loaded.
    void load();

    /// @brief Handles key events and calls the appropriate keybinds.
    void update();

    /// @brief Saves the keybinds to the configuration file.
    void save();

    /* == Utility function for keybind creation == */

    /// @brief Creates context menu for adding keybinds to hacks.
    /// @param id The ID of the keybinding.
    /// @param label The label of the keybinding.
    /// @param callback The callback to call when the keybinding is pressed.
    void addMenuKeybind(const std::string& id, const std::string& label, const std::function<void()>& callback);
}