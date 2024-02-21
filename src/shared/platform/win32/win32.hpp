#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <map>
#include <utility>
#include <vector>
#include <algorithm>
#include <shlobj.h>
#include <shellapi.h>

#include "4gb_patch.hpp"
#include "dll_inject.hpp"

#include <gd.hpp>

namespace openhack::utils {
    /// @brief Gets the current directory of the running executable.
    /// @return The current directory.
    inline std::string getCurrentDirectory() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
    }

    /// @brief Gets the save directory of Geometry Dash.
    /// @return The save directory.
    inline std::string getSaveDirectory() {
        char buffer[MAX_PATH];
        SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, buffer);
        std::string path = std::string(buffer) + "\\GeometryDash";
        return path;
    }

    /// @brief Opens a directory in the file explorer.
    /// @param path The path of the directory.
    inline void openDirectory(const char *path) {
        ShellExecuteA(nullptr, "open", path, nullptr, nullptr, SW_SHOWNORMAL);
    }

    /// @brief Opens a URL in the default web browser.
    /// @param url The URL to open.
    inline void openURL(const char *url) {
        ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOW);
    }

    /// @brief Gets the handle of the module with the given name.
    /// @param module The name of the module.
    /// @return The handle of the module.
    inline uintptr_t getModuleHandle(const char *module = nullptr) {
        return reinterpret_cast<uintptr_t>(GetModuleHandleA(module));
    }

    /// @brief Write bytes to the game's memory.
    /// @param address The address to write to.
    /// @param bytes The bytes to write.
    /// @return True if the bytes were successfully written.
    inline bool patchMemory(uintptr_t address, const std::vector<uint8_t> &bytes) {
        DWORD oldProtect;
        if (VirtualProtect((LPVOID) address, bytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            memcpy((void *) address, bytes.data(), bytes.size());
            VirtualProtect((LPVOID) address, bytes.size(), oldProtect, &oldProtect);
            return true;
        }
        return false;
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

    /// @brief Read bytes from the game's memory.
    /// @param address The address to read from.
    /// @param size The amount of bytes to read.
    /// @return The bytes read from the memory.
    inline std::vector<uint8_t> readMemory(uintptr_t address, size_t size) {
        std::vector<uint8_t> buffer(size);
        memcpy(buffer.data(), (void *) address, size);
        return buffer;
    }

    inline std::string gameVersion;

    /// @brief Automatically checks the game version and returns it.
    /// @return The game version.
    inline std::string getGameVersion() {
        if (!gameVersion.empty())
            return gameVersion;

        HMODULE module = GetModuleHandleA(nullptr);
        auto dosHeader = (PIMAGE_DOS_HEADER) module;

        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            gameVersion = "Unknown";
            return gameVersion;
        }

        auto ntHeaders = (PIMAGE_NT_HEADERS) ((uint8_t *) module + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
            gameVersion = "Unknown";
            return gameVersion;
        }

        std::map<uint32_t, std::string> versionsMap = {
                {1419173053, "1.900"},
                {1419880840, "1.910"},
                {1421745341, "1.920"},
                {1440638199, "2.000"},
                {1440643927, "2.001"},
                {1443053232, "2.010"},
                {1443077847, "2.011"},
                {1443077847, "2.020"},
                {1484612867, "2.100"},
                {1484626658, "2.101"},
                {1484737207, "2.102"},
                {1510526914, "2.110"},
                {1510538091, "2.111"},
                {1510619253, "2.112"},
                {1511220108, "2.113"},
                {1702921605, "2.200"},
                {1704582672, "2.201"},
                {1704601266, "2.202"},
                {1704948277, "2.203"},
                {1705041028, "2.204"},
        };

        uint32_t timestamp = ntHeaders->FileHeader.TimeDateStamp;
        auto version = versionsMap.find(timestamp);
        if (version != versionsMap.end()) {
            gameVersion = version->second;
            return gameVersion;
        }

        // check if larger than latest version
        if (timestamp > versionsMap.rbegin()->first) {
            // split by dot
            std::vector<std::string> version;
            std::string version_str = versionsMap.rbegin()->second;
            std::string delimiter = ".";
            size_t pos;
            std::string token;
            while ((pos = version_str.find(delimiter)) != std::string::npos) {
                token = version_str.substr(0, pos);
                version.push_back(token);
                version_str.erase(0, pos + delimiter.length());
            }
            version.push_back(version_str);

            // increment last number
            int32_t last = atoi(version[1].c_str());
            last++;
            version[1] = std::to_string(last);

            // join by dot
            gameVersion = version[0] + "." + version[1];
            return gameVersion;
        }

        gameVersion = "Unknown";
        return gameVersion;
    }

    inline std::map<uint32_t, std::string> KEY_NAMES = {
            {0,                      "..."}, // unset key
            {VK_LBUTTON,             "LMB"},
            {VK_RBUTTON,             "RMB"},
            {VK_MBUTTON,             "MMB"},
            {VK_XBUTTON1,            "X1"},
            {VK_XBUTTON2,            "X2"},
            {VK_BACK,                "Backspace"},
            {VK_TAB,                 "Tab"},
            {VK_CLEAR,               "Clear"},
            {VK_RETURN,              "Enter"},
            {VK_SHIFT,               "Shift"},
            {VK_CONTROL,             "Ctrl"},
            {VK_MENU,                "Alt"},
            {VK_PAUSE,               "Pause"},
            {VK_CAPITAL,             "Caps Lock"},
            {VK_ESCAPE,              "Esc"},
            {VK_SPACE,               "Space"},
            {VK_PRIOR,               "Page Up"},
            {VK_NEXT,                "Page Down"},
            {VK_END,                 "End"},
            {VK_HOME,                "Home"},
            {VK_LEFT,                "Left"},
            {VK_UP,                  "Up"},
            {VK_RIGHT,               "Right"},
            {VK_DOWN,                "Down"},
            {VK_SELECT,              "Select"},
            {VK_PRINT,               "Print"},
            {VK_EXECUTE,             "Execute"},
            {VK_SNAPSHOT,            "Print Screen"},
            {VK_INSERT,              "Insert"},
            {VK_DELETE,              "Delete"},
            {VK_HELP,                "Help"},
            {VK_LWIN,                "Left Windows"},
            {VK_RWIN,                "Right Windows"},
            {VK_APPS,                "Applications"},
            {VK_SLEEP,               "Sleep"},
            {VK_NUMPAD0,             "Numpad 0"},
            {VK_NUMPAD1,             "Numpad 1"},
            {VK_NUMPAD2,             "Numpad 2"},
            {VK_NUMPAD3,             "Numpad 3"},
            {VK_NUMPAD4,             "Numpad 4"},
            {VK_NUMPAD5,             "Numpad 5"},
            {VK_NUMPAD6,             "Numpad 6"},
            {VK_NUMPAD7,             "Numpad 7"},
            {VK_NUMPAD8,             "Numpad 8"},
            {VK_NUMPAD9,             "Numpad 9"},
            {VK_MULTIPLY,            "Numpad *"},
            {VK_ADD,                 "Numpad +"},
            {VK_SEPARATOR,           "Numpad Separator"},
            {VK_SUBTRACT,            "Numpad -"},
            {VK_DECIMAL,             "Numpad ."},
            {VK_DIVIDE,              "Numpad /"},
            {VK_F1,                  "F1"},
            {VK_F2,                  "F2"},
            {VK_F3,                  "F3"},
            {VK_F4,                  "F4"},
            {VK_F5,                  "F5"},
            {VK_F6,                  "F6"},
            {VK_F7,                  "F7"},
            {VK_F8,                  "F8"},
            {VK_F9,                  "F9"},
            {VK_F10,                 "F10"},
            {VK_F11,                 "F11"},
            {VK_F12,                 "F12"},
            {VK_F13,                 "F13"},
            {VK_F14,                 "F14"},
            {VK_F15,                 "F15"},
            {VK_F16,                 "F16"},
            {VK_F17,                 "F17"},
            {VK_F18,                 "F18"},
            {VK_F19,                 "F19"},
            {VK_F20,                 "F20"},
            {VK_F21,                 "F21"},
            {VK_F22,                 "F22"},
            {VK_F23,                 "F23"},
            {VK_F24,                 "F24"},
            {VK_NUMLOCK,             "Num Lock"},
            {VK_SCROLL,              "Scroll Lock"},
            {VK_LSHIFT,              "Left Shift"},
            {VK_RSHIFT,              "Right Shift"},
            {VK_LCONTROL,            "Left Ctrl"},
            {VK_RCONTROL,            "Right Ctrl"},
            {VK_LMENU,               "Left Alt"},
            {VK_RMENU,               "Right Alt"},
            {VK_BROWSER_BACK,        "Browser Back"},
            {VK_BROWSER_FORWARD,     "Browser Forward"},
            {VK_BROWSER_REFRESH,     "Browser Refresh"},
            {VK_BROWSER_STOP,        "Browser Stop"},
            {VK_BROWSER_SEARCH,      "Browser Search"},
            {VK_BROWSER_FAVORITES,   "Browser Favorites"},
            {VK_BROWSER_HOME,        "Browser Home"},
            {VK_VOLUME_MUTE,         "Volume Mute"},
            {VK_VOLUME_DOWN,         "Volume Down"},
            {VK_VOLUME_UP,           "Volume Up"},
            {VK_MEDIA_NEXT_TRACK,    "Next Track"},
            {VK_MEDIA_PREV_TRACK,    "Previous Track"},
            {VK_MEDIA_STOP,          "Stop"},
            {VK_MEDIA_PLAY_PAUSE,    "Play/Pause"},
            {VK_LAUNCH_MAIL,         "Launch Mail"},
            {VK_LAUNCH_MEDIA_SELECT, "Select Media"},
            {VK_LAUNCH_APP1,         "Launch App 1"},
            {VK_LAUNCH_APP2,         "Launch App 2"},
            {VK_OEM_1,               ";"},
            {VK_OEM_PLUS,            "+"},
            {VK_OEM_COMMA,           ","},
            {VK_OEM_MINUS,           "-"},
            {VK_OEM_PERIOD,          "."},
            {VK_OEM_2,               "/"},
            {VK_OEM_3,               "`"},
            {VK_OEM_4,               "["},
            {VK_OEM_5,               "\\"},
            {VK_OEM_6,               "]"},
            {VK_OEM_7,               "'"},
            {VK_OEM_8,               "OEM 8"},
            {VK_OEM_102,             "OEM 102"},
            {VK_PROCESSKEY,          "IME Process"},
            {VK_PACKET,              "Packet"},
            {VK_ATTN,                "Attn"},
            {VK_CRSEL,               "CrSel"},
            {VK_EXSEL,               "ExSel"},
            {VK_EREOF,               "Erase EOF"},
            {VK_PLAY,                "Play"},
            {VK_ZOOM,                "Zoom"},
            {VK_NONAME,              "NoName"},
            {VK_PA1,                 "PA1"},
            {VK_OEM_CLEAR,           "Clear"},
    };

    /// @brief Convert a key code to a key name.
    /// @param keycode The key code.
    /// @return The key name.
    inline std::string getKeyName(uint32_t keycode) {
        // find key in map
        for (auto &key: KEY_NAMES) {
            if (key.first == keycode) {
                return key.second;
            }
        }

        // check if key is a letter
        if (keycode >= 0x41 && keycode <= 0x5A) {
            return std::string(1, (char) keycode);
        }

        // check if key is a number
        if (keycode >= 0x30 && keycode <= 0x39) {
            return std::string(1, (char) keycode);
        }

        return "Unknown";
    }

    /// @brief Convert a key name to a key code.
    /// @param key The key name.
    /// @return The key code.
    inline uint32_t getKeyCode(std::string key) {
        // find key in map
        for (auto &key_: KEY_NAMES) {
            // compare lowercase strings
            std::string key_lower = key;
            std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(), ::tolower);
            std::string key_lower_ = key_.second;
            std::transform(key_lower_.begin(), key_lower_.end(), key_lower_.begin(), ::tolower);
            if (key_lower == key_lower_) {
                return key_.first;
            }
        }

        // check if key is a letter
        if (key.length() == 1) {
            return (uint32_t) key[0];
        }

        return 0;
    }

    inline HWND WINDOW_HANDLE = nullptr;

    /// @brief Gets the window handle of the game.
    inline HWND getWindowHandle() {
        if (WINDOW_HANDLE != nullptr)
            return WINDOW_HANDLE;

        auto *director = gd::cocos2d::CCDirector::sharedDirector();
        auto *window = director->getOpenGLView()->getWindow();
        WINDOW_HANDLE = WindowFromDC(*reinterpret_cast<HDC *>(reinterpret_cast<uintptr_t>(window) + 0x244));
        return WINDOW_HANDLE;
    }

    /// @brief Resets the window handle.
    inline void resetWindowHandle() {
        WINDOW_HANDLE = nullptr;
    }

    inline std::map<uint32_t, bool> KEY_STATES;
    inline std::map<uint32_t, bool> KEY_STATES_PREVIOUS;

    /// @brief Check whether a key is down.
    inline bool isKeyDown(uint32_t keycode) {
        return GetAsyncKeyState(keycode) & 0x8000;
    }

    /// @brief Check whether a key is down.
    inline bool isKeyDown(std::string key) {
        return isKeyDown(getKeyCode(std::move(key)));
    }

    /// @brief Check whether a key was pressed during the current frame.
    /// @param keycode The key code.
    /// @return True if the key was pressed.
    inline bool isKeyPressed(uint32_t keycode) {
        // if window is not focused
        if (GetForegroundWindow() != getWindowHandle())
            return false;

        // if state changed
        if (KEY_STATES[keycode] ^ KEY_STATES_PREVIOUS[keycode]) {
            // return current state
            return KEY_STATES[keycode];
        }

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
        // if window is not focused
        if (GetForegroundWindow() != getWindowHandle())
            return false;

        // if state changed
        if (KEY_STATES[keycode] ^ KEY_STATES_PREVIOUS[keycode]) {
            // return current state
            return !KEY_STATES[keycode];
        }

        return false;
    }

    /// @brief Check whether a key was released during the current frame.
    /// @param key Key name.
    /// @return True if the key was released.
    inline bool isKeyReleased(std::string key) {
        return isKeyReleased(getKeyCode(std::move(key)));
    }

    /// @brief Needs to be called every frame to update the key states.
    inline void resetKeyStates() {
        // set previous states
        for (auto &key: KEY_STATES) {
            KEY_STATES_PREVIOUS[key.first] = key.second;
        }

        // reset all keys
        for (int i = 0; i < 256; i++) {
            KEY_STATES[i] = GetAsyncKeyState(i) & 0x8000;
        }
    }
}