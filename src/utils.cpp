#include "utils.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <map>
#include <vector>
#include <algorithm>
#include <shlobj.h>
#include <shellapi.h>

#include <curl/curl.h>
#include "logger.h"
#include "hook.h"

namespace utils
{
    uintptr_t base_addr, cocos_addr;
    bool is_2_200;

    void init()
    {
        base_addr = (uintptr_t)GetModuleHandleA(NULL);
        cocos_addr = (uintptr_t)GetModuleHandleA("libcocos2d.dll");
        is_2_200 = compare_version("=2.200");
    }

    void set_console_title(const char *title)
    {
        SetConsoleTitleA(title);
    }

    std::string get_current_directory()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
    }

    std::string get_save_directory()
    {
        char buffer[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buffer);
        std::string path = std::string(buffer) + "\\GeometryDash";
        return path;
    }

    void open_folder(const char *path)
    {
        ShellExecuteA(NULL, "open", path, NULL, NULL, SW_SHOWNORMAL);
    }

    std::map<uint32_t, std::string> versions_map = {
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

    char *game_version = NULL;

    const char *get_game_version()
    {
        if (game_version != NULL)
            return game_version;

        HMODULE module = GetModuleHandleA(NULL);
        auto dos_header = (PIMAGE_DOS_HEADER)module;

        if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
        {
            game_version = "unknown";
            return game_version;
        }

        auto nt_headers = (PIMAGE_NT_HEADERS)((uint8_t *)module + dos_header->e_lfanew);

        if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
        {
            game_version = "unknown";
            return game_version;
        }

        uint32_t timestamp = nt_headers->FileHeader.TimeDateStamp;
        auto version = versions_map.find(timestamp);
        if (version != versions_map.end())
        {
            game_version = (char *)version->second.c_str();
            return game_version;
        }

        // check if larger than latest version
        if (timestamp > versions_map.rbegin()->first)
        {
            L_TRACE("New version found: {}", timestamp);

            // split by dot
            std::vector<std::string> version;
            std::string version_str = versions_map.rbegin()->second;
            std::string delimiter = ".";
            size_t pos = 0;
            std::string token;
            while ((pos = version_str.find(delimiter)) != std::string::npos)
            {
                token = version_str.substr(0, pos);
                version.push_back(token);
                version_str.erase(0, pos + delimiter.length());
            }
            version.push_back(version_str);

            // increment last number
            int32_t last = atoi(version[1].c_str());
            last++;
            version[1] = std::to_string(last);

            if (game_version != NULL)
                delete game_version;
            game_version = new char[version[0].length() + version[1].length() + 2];
            sprintf(game_version, "%s.%s", version[0].c_str(), version[1].c_str());
            return game_version;
        }

        game_version = "unknown";
        return game_version;
    }

    bool compare_version(const char *version_filter)
    {
        // split by comma
        std::vector<std::string> versions;
        std::string version_filter_str = version_filter;
        std::string delimiter = ",";
        size_t pos = 0;
        std::string token;
        while ((pos = version_filter_str.find(delimiter)) != std::string::npos)
        {
            token = version_filter_str.substr(0, pos);
            versions.push_back(token);
            version_filter_str.erase(0, pos + delimiter.length());
        }
        versions.push_back(version_filter_str);

        // get current version
        const char *current_version = get_game_version();

        // check if any version matches
        bool match = false;
        for (auto version : versions)
        {
            // check if version is a range
            if (version.find("-") != std::string::npos)
            {
                // split by dash
                std::vector<std::string> range;
                std::string range_str = version;
                std::string delimiter = "-";
                size_t pos = 0;
                std::string token;
                while ((pos = range_str.find(delimiter)) != std::string::npos)
                {
                    token = range_str.substr(0, pos);
                    range.push_back(token);
                    range_str.erase(0, pos + delimiter.length());
                }
                range.push_back(range_str);

                // check if current version is in range
                if (range.size() == 2)
                {
                    if (strcmp(current_version, range[0].c_str()) >= 0 && strcmp(current_version, range[1].c_str()) <= 0)
                    {
                        match = true;
                        break;
                    }
                }
            }
            else
            {
                // get operator
                char op = version.at(0);
                std::string ver = version.substr(1);
                std::string operation;
                switch (op)
                {
                case '=':
                    operation = "==";
                    break;
                case '!':
                    operation = "!=";
                    break;
                case '<':
                    // check if <=
                    if (version[1] == '=')
                    {
                        operation = "<=";
                        ver = ver.substr(1);
                    }
                    else
                    {
                        operation = "<";
                    }
                    break;
                case '>':
                    // check if >=
                    if (version[1] == '=')
                    {
                        operation = ">=";
                        ver = ver.substr(1);
                    }
                    else
                    {
                        operation = ">";
                    }
                    break;
                default:
                    operation = "==";
                    break;
                }

                // use operation to compare versions
                int32_t result = strcmp(current_version, ver.c_str());
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

    std::map<uint32_t, bool> key_states;
    std::map<uint32_t, bool> key_states_previous;
    bool is_key_pressed(uint32_t keycode)
    {
        // if window is not focused
        if (GetForegroundWindow() != hook::window_handle)
            return false;

        // if state changed
        if (key_states[keycode] ^ key_states_previous[keycode])
        {
            // return current state
            return key_states[keycode];
        }

        return false;
    }

    void reset_key_states()
    {
        // set previous states
        for (auto &key : key_states)
        {
            key_states_previous[key.first] = key.second;
        }

        // reset all keys
        for (int i = 0; i < 256; i++)
        {
            key_states[i] = GetAsyncKeyState(i) & 0x8000;
        }
    }

    std::map<uint32_t, std::string> key_names = {
        {0, "..."}, // unset key
        {VK_LBUTTON, "LMB"},
        {VK_RBUTTON, "RMB"},
        {VK_MBUTTON, "MMB"},
        {VK_XBUTTON1, "X1"},
        {VK_XBUTTON2, "X2"},
        {VK_BACK, "Backspace"},
        {VK_TAB, "Tab"},
        {VK_CLEAR, "Clear"},
        {VK_RETURN, "Enter"},
        {VK_SHIFT, "Shift"},
        {VK_CONTROL, "Ctrl"},
        {VK_MENU, "Alt"},
        {VK_PAUSE, "Pause"},
        {VK_CAPITAL, "Caps Lock"},
        {VK_ESCAPE, "Esc"},
        {VK_SPACE, "Space"},
        {VK_PRIOR, "Page Up"},
        {VK_NEXT, "Page Down"},
        {VK_END, "End"},
        {VK_HOME, "Home"},
        {VK_LEFT, "Left"},
        {VK_UP, "Up"},
        {VK_RIGHT, "Right"},
        {VK_DOWN, "Down"},
        {VK_SELECT, "Select"},
        {VK_PRINT, "Print"},
        {VK_EXECUTE, "Execute"},
        {VK_SNAPSHOT, "Print Screen"},
        {VK_INSERT, "Insert"},
        {VK_DELETE, "Delete"},
        {VK_HELP, "Help"},
        {VK_LWIN, "Left Windows"},
        {VK_RWIN, "Right Windows"},
        {VK_APPS, "Applications"},
        {VK_SLEEP, "Sleep"},
        {VK_NUMPAD0, "Numpad 0"},
        {VK_NUMPAD1, "Numpad 1"},
        {VK_NUMPAD2, "Numpad 2"},
        {VK_NUMPAD3, "Numpad 3"},
        {VK_NUMPAD4, "Numpad 4"},
        {VK_NUMPAD5, "Numpad 5"},
        {VK_NUMPAD6, "Numpad 6"},
        {VK_NUMPAD7, "Numpad 7"},
        {VK_NUMPAD8, "Numpad 8"},
        {VK_NUMPAD9, "Numpad 9"},
        {VK_MULTIPLY, "Numpad *"},
        {VK_ADD, "Numpad +"},
        {VK_SEPARATOR, "Numpad Separator"},
        {VK_SUBTRACT, "Numpad -"},
        {VK_DECIMAL, "Numpad ."},
        {VK_DIVIDE, "Numpad /"},
        {VK_F1, "F1"},
        {VK_F2, "F2"},
        {VK_F3, "F3"},
        {VK_F4, "F4"},
        {VK_F5, "F5"},
        {VK_F6, "F6"},
        {VK_F7, "F7"},
        {VK_F8, "F8"},
        {VK_F9, "F9"},
        {VK_F10, "F10"},
        {VK_F11, "F11"},
        {VK_F12, "F12"},
        {VK_F13, "F13"},
        {VK_F14, "F14"},
        {VK_F15, "F15"},
        {VK_F16, "F16"},
        {VK_F17, "F17"},
        {VK_F18, "F18"},
        {VK_F19, "F19"},
        {VK_F20, "F20"},
        {VK_F21, "F21"},
        {VK_F22, "F22"},
        {VK_F23, "F23"},
        {VK_F24, "F24"},
        {VK_NUMLOCK, "Num Lock"},
        {VK_SCROLL, "Scroll Lock"},
        {VK_LSHIFT, "Left Shift"},
        {VK_RSHIFT, "Right Shift"},
        {VK_LCONTROL, "Left Ctrl"},
        {VK_RCONTROL, "Right Ctrl"},
        {VK_LMENU, "Left Alt"},
        {VK_RMENU, "Right Alt"},
        {VK_BROWSER_BACK, "Browser Back"},
        {VK_BROWSER_FORWARD, "Browser Forward"},
        {VK_BROWSER_REFRESH, "Browser Refresh"},
        {VK_BROWSER_STOP, "Browser Stop"},
        {VK_BROWSER_SEARCH, "Browser Search"},
        {VK_BROWSER_FAVORITES, "Browser Favorites"},
        {VK_BROWSER_HOME, "Browser Home"},
        {VK_VOLUME_MUTE, "Volume Mute"},
        {VK_VOLUME_DOWN, "Volume Down"},
        {VK_VOLUME_UP, "Volume Up"},
        {VK_MEDIA_NEXT_TRACK, "Next Track"},
        {VK_MEDIA_PREV_TRACK, "Previous Track"},
        {VK_MEDIA_STOP, "Stop"},
        {VK_MEDIA_PLAY_PAUSE, "Play/Pause"},
        {VK_LAUNCH_MAIL, "Launch Mail"},
        {VK_LAUNCH_MEDIA_SELECT, "Select Media"},
        {VK_LAUNCH_APP1, "Launch App 1"},
        {VK_LAUNCH_APP2, "Launch App 2"},
        {VK_OEM_1, ";"},
        {VK_OEM_PLUS, "+"},
        {VK_OEM_COMMA, ","},
        {VK_OEM_MINUS, "-"},
        {VK_OEM_PERIOD, "."},
        {VK_OEM_2, "/"},
        {VK_OEM_3, "`"},
        {VK_OEM_4, "["},
        {VK_OEM_5, "\\"},
        {VK_OEM_6, "]"},
        {VK_OEM_7, "'"},
        {VK_OEM_8, "OEM 8"},
        {VK_OEM_102, "OEM 102"},
        {VK_PROCESSKEY, "IME Process"},
        {VK_PACKET, "Packet"},
        {VK_ATTN, "Attn"},
        {VK_CRSEL, "CrSel"},
        {VK_EXSEL, "ExSel"},
        {VK_EREOF, "Erase EOF"},
        {VK_PLAY, "Play"},
        {VK_ZOOM, "Zoom"},
        {VK_NONAME, "NoName"},
        {VK_PA1, "PA1"},
        {VK_OEM_CLEAR, "Clear"},
    };

    std::string key_name(uint32_t keycode)
    {
        // find key in map
        for (auto &key : key_names)
        {
            if (key.first == keycode)
            {
                return key.second;
            }
        }

        // check if key is a letter
        if (keycode >= 0x41 && keycode <= 0x5A)
        {
            return std::string(1, (char)keycode);
        }

        // check if key is a number
        if (keycode >= 0x30 && keycode <= 0x39)
        {
            return std::string(1, (char)keycode);
        }

        return "Unknown";
    }

    uint32_t decode_key(std::string key)
    {
        // find key in map
        for (auto &key_ : key_names)
        {
            // compare lowercase strings
            std::string key_lower = key;
            std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(), ::tolower);
            std::string key_lower_ = key_.second;
            std::transform(key_lower_.begin(), key_lower_.end(), key_lower_.begin(), ::tolower);
            if (key_lower == key_lower_)
            {
                return key_.first;
            }
        }

        // check if key is a letter
        if (key.length() == 1)
        {
            return (uint32_t)key[0];
        }

        return 0;
    }

    size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *output)
    {
        output->append((char *)contents, size * nmemb);
        return size * nmemb;
    }

    bool curl_initialized = false;

    std::string get_request(const char *url)
    {
        if (!curl_initialized)
        {
            curl_global_init(CURL_GLOBAL_ALL);
            curl_initialized = true;
        }

        CURL *curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "openhack");
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                L_ERROR("curl_easy_perform() failed: {}", curl_easy_strerror(res));
            }

            curl_easy_cleanup(curl);
        }

        return readBuffer;
    }

    struct progress_data
    {
        float *progress;
        CURL *curl;
        std::ofstream *file;
    };

    size_t write_callback_progress(void *contents, size_t size, size_t nmemb, progress_data *data)
    {
        data->file->write((char *)contents, size * nmemb);
        if (data->progress != nullptr)
        {
            double dltotal = 0;
            double dlnow = 0;
            curl_easy_getinfo(data->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &dltotal);
            curl_easy_getinfo(data->curl, CURLINFO_SIZE_DOWNLOAD, &dlnow);
            *data->progress = (float)(dlnow / dltotal);
        }
        return size * nmemb;
    }

    bool download_file(const char *url, const char *path, float *progress)
    {
        if (!curl_initialized)
        {
            curl_global_init(CURL_GLOBAL_ALL);
            curl_initialized = true;
        }

        CURL *curl;
        CURLcode res;
        std::ofstream file(path, std::ios::binary);

        curl = curl_easy_init();
        if (curl)
        {
            progress_data data;
            data.progress = progress;
            data.curl = curl;
            data.file = &file;

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::write_callback_progress);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "openhack");
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                L_ERROR("curl_easy_perform() failed: {}", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                file.close();
                return false;
            }

            curl_easy_cleanup(curl);
        }

        file.close();

        return true;
    }

    void extract_zip(const char *zip_path, const char *output_path)
    {
        std::string command = "powershell -Command \"Expand-Archive -Path '";
        command += zip_path;
        command += "' -DestinationPath '";
        command += output_path;
        command += "' -Force\"";
        system(command.c_str());
    }
}