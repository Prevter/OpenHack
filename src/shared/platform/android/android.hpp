#pragma once

#include <string>
#include <map>
#include <utility>
#include <vector>
#include <algorithm>
#include <filesystem>

#include <dlfcn.h>
#include <sys/stat.h>

namespace openhack::utils {
    /// @brief Opens a directory in the file explorer.
    /// @param path The path of the directory.
    inline void openDirectory(const char *path) {
        geode::utils::file::openFolder(path);
    }

    /// @brief Opens a URL in the default web browser.
    /// @param url The URL to open.
    inline void openURL(const std::string& url) {
        cocos2d::CCApplication::get()->openURL(url.c_str());
    }

    /// @brief Gets the handle of the module with the given name.
    /// @param module The name of the module.
    /// @return The handle of the module.
    inline uintptr_t getModuleHandle(const char *module = nullptr) {
        // if module is nullptr or libcocos2d.dll, return the base address of the game
        if (!module || !strcmp(module, "libcocos2d.dll")) {
            return geode::base::get();
        }
        return 0;
    }

    inline sinaps::module_t getModule(const std::string& module) {
        uintptr_t base = getModuleHandle(module.c_str());
        if (!base) return {module, 0, 0};

        struct stat fileStat{};
        if (stat(module.c_str(), &fileStat) == 0) {
            return {module, base, static_cast<uintptr_t>(fileStat.st_size)};
        }

        return {module, base, 0};
    }

    /// @brief Read bytes from the game's memory.
    /// @param address The address to read from.
    /// @param size The amount of bytes to read.
    /// @return The bytes read from the memory.
    inline std::vector<uint8_t> readMemory(uintptr_t address, size_t size) {
        std::vector<uint8_t> buffer(size);
        memcpy(buffer.data(), (void *) address, size);
        return buffer;
    }

    /// @return The game version.
    inline std::string getGameVersion() {
        return GEODE_STR(GEODE_GD_VERSION);
    }

    /// @brief Convert a key code to a key name.
    /// @param keycode The key code.
    /// @return The key name.
    inline std::string getKeyName(uint32_t keycode) {
        return "Unknown";
    }

    /// @brief Convert a key name to a key code.
    /// @param key The key name.
    /// @return The key code.
    inline uint32_t getKeyCode(std::string key) {
        return 0;
    }

    /// @brief Check whether a key is down.
    inline bool isKeyDown(uint32_t keycode) {
        return false;
    }

    /// @brief Check whether a key is down.
    inline bool isKeyDown(std::string key) {
        return isKeyDown(getKeyCode(std::move(key)));
    }

    /// @brief Check whether a key was pressed during the current frame.
    /// @param keycode The key code.
    /// @return True if the key was pressed.
    inline bool isKeyPressed(uint32_t keycode) {
        return false;
    }

    /// @brief Check whether a key was pressed during the current frame.
    /// @param key Key name.
    /// @return True if the key was pressed.
    inline bool isKeyPressed(std::string key) {
        return isKeyPressed(getKeyCode(std::move(key)));
    }

    /// @brief Check whether a key was released during the current frame.
    /// @param keycode The key code.
    /// @return True if the key was released.
    inline bool isKeyReleased(uint32_t keycode) {
        return false;
    }

    /// @brief Check whether a key was released during the current frame.
    /// @param key Key name.
    /// @return True if the key was released.
    inline bool isKeyReleased(std::string key) {
        return isKeyReleased(getKeyCode(std::move(key)));
    }

    /// @brief Write bytes to the game's memory.
    /// @param address The address to write to.
    /// @param bytes The bytes to write.
    /// @return True if the bytes were successfully written.
    inline bool patchMemory(uintptr_t address, const std::vector<uint8_t> &bytes) {
        return sinaps::mem::write(reinterpret_cast<void *>(address), bytes.data(), bytes.size());
    }

    /// @brief Write a value to the game's memory.
    /// @tparam T The type of the value.
    /// @param address The address to write to.
    /// @param value The value to write.
    /// @return True if the value was successfully written.
    template <typename T>
    inline bool writeMemory(uintptr_t address, T value) {
        return patchMemory(address, std::vector<uint8_t>((uint8_t *) &value, (uint8_t *) &value + sizeof(T)));
    }
}