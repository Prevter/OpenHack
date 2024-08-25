#include "win32.hpp"

namespace openhack::utils {
    std::string getCurrentDirectory() {
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
    }

    std::string getSaveDirectory() {
        char buffer[MAX_PATH];
        SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, buffer);
        std::string path = std::string(buffer) + "\\GeometryDash";
        return path;
    }

    void openDirectory(const char *path) {
        ShellExecuteA(nullptr, "open", path, nullptr, nullptr, SW_SHOWNORMAL);
    }

    void openURL(const char *url) {
        ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOW);
    }

    uintptr_t getModuleHandle(const char *module) {
        return reinterpret_cast<uintptr_t>(GetModuleHandleA(module));
    }

    bool patchMemory(uintptr_t address, const std::vector<uint8_t> &bytes) {
        DWORD oldProtect;
        if (VirtualProtect((LPVOID) address, bytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            memcpy((void *) address, bytes.data(), bytes.size());
            VirtualProtect((LPVOID) address, bytes.size(), oldProtect, &oldProtect);
            return true;
        }
        return false;
    }

    std::vector<uint8_t> readMemory(uintptr_t address, size_t size) {
        std::vector<uint8_t> buffer(size);
        memcpy(buffer.data(), (void *) address, size);
        return buffer;
    }

    /// @brief Automatically checks the game version and returns it.
    /// @return The game version.
    std::string getGameVersion() {
        static std::string gameVersion;
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
                {1717243515, "2.206"}
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

    const std::map<uint32_t, std::string>& getKeyNames() {
        static std::map<uint32_t, std::string> KEY_NAMES = {
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
        return KEY_NAMES;
    }

    std::string getKeyName(uint32_t keycode) {
        // find key in map
        for (auto &key: getKeyNames()) {
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

    uint32_t getKeyCode(std::string_view key) {
        // find key in map
        std::string key_lower = std::string(key);
        std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(), ::tolower);

        for (auto &key_: getKeyNames()) {
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

    HWND WINDOW_HANDLE = nullptr;

    HWND getWindowHandle() {
        if (WINDOW_HANDLE != nullptr)
            return WINDOW_HANDLE;

        auto *director = cocos2d::CCDirector::sharedDirector();
        auto *window = director->getOpenGLView()->getWindow();
        WINDOW_HANDLE = WindowFromDC(*reinterpret_cast<HDC *>(reinterpret_cast<uintptr_t>(window) + 0x278));
        return WINDOW_HANDLE;
    }

    void resetWindowHandle() {
        WINDOW_HANDLE = nullptr;
    }

    std::map<uint32_t, bool>& getKeyStates() {
        static std::map<uint32_t, bool> keyStates;
        return keyStates;
    }

    std::map<uint32_t, bool>& getKeyStatesPrevious() {
        static std::map<uint32_t, bool> keyStatesPrevious;
        return keyStatesPrevious;
    }

    bool isKeyDown(uint32_t keycode) {
        return GetAsyncKeyState(keycode) & 0x8000;
    }

    bool isKeyDown(std::string_view key) {
        return isKeyDown(getKeyCode(key));
    }

    bool isKeyPressed(uint32_t keycode) {
        // if window is not focused
        if (GetForegroundWindow() != getWindowHandle())
            return false;

        // if state changed
        if (getKeyStates()[keycode] ^ getKeyStatesPrevious()[keycode]) {
            // return current state
            return getKeyStates()[keycode];
        }

        return false;
    }

    bool isKeyPressed(std::string_view key) {
        return isKeyPressed(getKeyCode(key));
    }

    bool isKeyReleased(uint32_t keycode) {
        // if window is not focused
        if (GetForegroundWindow() != getWindowHandle())
            return false;

        // if state changed
        if (getKeyStates()[keycode] ^ getKeyStatesPrevious()[keycode]) {
            // return current state
            return !getKeyStates()[keycode];
        }

        return false;
    }

    bool isKeyReleased(std::string_view key) {
        return isKeyReleased(getKeyCode(key));
    }

    void resetKeyStates() {
        // set previous states
        for (auto& [key, value]: getKeyStates()) {
            getKeyStatesPrevious()[key] = value;
        }

        // reset all keys
        for (int i = 0; i < 256; i++) {
            getKeyStates()[i] = GetAsyncKeyState(i) & 0x8000;
        }
    }

    void pressKey(uint32_t keycode) {
        keybd_event(keycode, 0, 0, 0);
        keybd_event(keycode, 0, KEYEVENTF_KEYUP, 0);
    }

    void pressKey(std::string_view key) {
        pressKey(getKeyCode(key));
    }

    std::filesystem::path filePickerDialog(const char *filter, const char *title) {
        OPENFILENAME ofn;
        char szFile[260] = {0};
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        ofn.lpstrTitle = title;
        GetOpenFileName(&ofn);
        return ofn.lpstrFile;
    }

    /// @brief Open a file dialog to save a file.
    std::filesystem::path fileSaveDialog(const char *filter, const char *title) {
        OPENFILENAME ofn;
        char szFile[260] = {0};
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        ofn.lpstrTitle = title;
        GetSaveFileName(&ofn);
        return ofn.lpstrFile;
    }
}