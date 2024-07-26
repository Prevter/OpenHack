#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <map>
#include <utility>
#include <vector>
#include <algorithm>
#include <shlobj.h>
#include <shellapi.h>
#include <filesystem>

#include "dll_inject.hpp"

namespace openhack::utils {
    /// @brief Gets the current directory of the running executable.
    /// @return The current directory.
    std::string getCurrentDirectory();

    /// @brief Gets the save directory of Geometry Dash.
    /// @return The save directory.
    std::string getSaveDirectory();

    /// @brief Opens a directory in the file explorer.
    /// @param path The path of the directory.
    void openDirectory(const char *path);

    /// @brief Opens a URL in the default web browser.
    /// @param url The URL to open.
    void openURL(const char *url);

    /// @brief Gets the handle of the module with the given name.
    /// @param module The name of the module.
    /// @return The handle of the module.
    uintptr_t getModuleHandle(const char *module = nullptr);

    /// @brief Write bytes to the game's memory.
    /// @param address The address to write to.
    /// @param bytes The bytes to write.
    /// @return True if the bytes were successfully written.
    bool patchMemory(uintptr_t address, const std::vector<uint8_t> &bytes);

    /// @brief Write a value to the game's memory.
    /// @tparam T The type of the value.
    /// @param address The address to write to.
    /// @param value The value to write.
    /// @return True if the value was successfully written.
    template <typename T>
    inline bool writeMemory(uintptr_t address, T value) {
        return patchMemory(address, std::vector<uint8_t>((uint8_t *) &value, (uint8_t *) &value + sizeof(T)));
    }

    /// @brief Read bytes from the game's memory.
    /// @param address The address to read from.
    /// @param size The amount of bytes to read.
    /// @return The bytes read from the memory.
    std::vector<uint8_t> readMemory(uintptr_t address, size_t size);

    /// @brief Automatically checks the game version and returns it.
    /// @return The game version.
    std::string getGameVersion();

    /// @brief Convert a key code to a key name.
    /// @param keycode The key code.
    /// @return The key name.
    std::string getKeyName(uint32_t keycode);

    /// @brief Convert a key name to a key code.
    /// @param key The key name.
    /// @return The key code.
    uint32_t getKeyCode(std::string key);

    /// @brief Gets the window handle of the game.
    HWND getWindowHandle();

    /// @brief Resets the window handle.
    void resetWindowHandle();

    /// @brief Check whether a key is down.
    bool isKeyDown(uint32_t keycode);

    /// @brief Check whether a key is down.
    bool isKeyDown(std::string key);

    /// @brief Check whether a key was pressed during the current frame.
    /// @param keycode The key code.
    /// @return True if the key was pressed.
    bool isKeyPressed(uint32_t keycode);

    /// @brief Check whether a key was pressed during the current frame.
    /// @param key Key name.
    /// @return True if the key was pressed.
    bool isKeyPressed(std::string key);

    /// @brief Check whether a key was released during the current frame.
    /// @param keycode The key code.
    /// @return True if the key was released.
    bool isKeyReleased(uint32_t keycode);

    /// @brief Check whether a key was released during the current frame.
    /// @param key Key name.
    /// @return True if the key was released.
    bool isKeyReleased(std::string key);

    /// @brief Needs to be called every frame to update the key states.
    void resetKeyStates();

    /// @brief Simulates a key press.
    /// @param keycode The key code.
    void pressKey(uint32_t keycode);

    /// @brief Simulates a key press.
    /// @param key The key name.
    void pressKey(std::string key);

    /// @brief Open a file dialog to select a file.
    std::filesystem::path filePickerDialog(const char *filter, const char *title);

    /// @brief Open a file dialog to save a file.
    std::filesystem::path fileSaveDialog(const char *filter, const char *title);
}