#include "utils.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <map>
#include <vector>

#include <curl/curl.h>
#include "logger.h"

namespace utils
{
    uintptr_t base_addr, cocos_addr;

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

    // I need a better way to do this
    uint32_t decode_key(std::string key)
    {
        if (key == "mouse1")
            return VK_LBUTTON;
        else if (key == "mouse2")
            return VK_RBUTTON;
        else if (key == "mouse3")
            return VK_MBUTTON;
        else if (key == "mouse4")
            return VK_XBUTTON1;
        else if (key == "mouse5")
            return VK_XBUTTON2;
        else if (key == "backspace")
            return VK_BACK;
        else if (key == "tab")
            return VK_TAB;
        else if (key == "enter")
            return VK_RETURN;
        else if (key == "shift")
            return VK_SHIFT;
        else if (key == "ctrl")
            return VK_CONTROL;
        else if (key == "alt")
            return VK_MENU;
        else if (key == "pause")
            return VK_PAUSE;
        else if (key == "capslock")
            return VK_CAPITAL;
        else if (key == "esc")
            return VK_ESCAPE;
        else if (key == "space")
            return VK_SPACE;
        else if (key == "pageup")
            return VK_PRIOR;
        else if (key == "pagedown")
            return VK_NEXT;
        else if (key == "end")
            return VK_END;
        else if (key == "home")
            return VK_HOME;
        else if (key == "left")
            return VK_LEFT;
        else if (key == "up")
            return VK_UP;
        else if (key == "right")
            return VK_RIGHT;
        else if (key == "down")
            return VK_DOWN;
        else if (key == "print")
            return VK_PRINT;
        else if (key == "printscreen")
            return VK_SNAPSHOT;
        else if (key == "insert")
            return VK_INSERT;
        else if (key == "delete")
            return VK_DELETE;
        else if (key == "lwin")
            return VK_LWIN;
        else if (key == "rwin")
            return VK_RWIN;
        else if (key == "apps")
            return VK_APPS;
        else if (key == "sleep")
            return VK_SLEEP;
        else if (key == "num0")
            return VK_NUMPAD0;
        else if (key == "num1")
            return VK_NUMPAD1;
        else if (key == "num2")
            return VK_NUMPAD2;
        else if (key == "num3")
            return VK_NUMPAD3;
        else if (key == "num4")
            return VK_NUMPAD4;
        else if (key == "num5")
            return VK_NUMPAD5;
        else if (key == "num6")
            return VK_NUMPAD6;
        else if (key == "num7")
            return VK_NUMPAD7;
        else if (key == "num8")
            return VK_NUMPAD8;
        else if (key == "num9")
            return VK_NUMPAD9;
        else if (key == "num*")
            return VK_MULTIPLY;
        else if (key == "num+")
            return VK_ADD;
        else if (key == "num-")
            return VK_SUBTRACT;
        else if (key == "num.")
            return VK_DECIMAL;
        else if (key == "num/")
            return VK_DIVIDE;
        else if (key == "f1")
            return VK_F1;
        else if (key == "f2")
            return VK_F2;
        else if (key == "f3")
            return VK_F3;
        else if (key == "f4")
            return VK_F4;
        else if (key == "f5")
            return VK_F5;
        else if (key == "f6")
            return VK_F6;
        else if (key == "f7")
            return VK_F7;
        else if (key == "f8")
            return VK_F8;
        else if (key == "f9")
            return VK_F9;
        else if (key == "f10")
            return VK_F10;
        else if (key == "f11")
            return VK_F11;
        else if (key == "f12")
            return VK_F12;
        else if (key == "numlock")
            return VK_NUMLOCK;
        else if (key == "scrolllock")
            return VK_SCROLL;
        else if (key == "lshift")
            return VK_LSHIFT;
        else if (key == "rshift")
            return VK_RSHIFT;
        else if (key == "lctrl")
            return VK_LCONTROL;
        else if (key == "rctrl")
            return VK_RCONTROL;
        else if (key == "lalt")
            return VK_LMENU;
        else if (key == "ralt")
            return VK_RMENU;
        else if (key == "browser_back")
            return VK_BROWSER_BACK;
        else if (key == "browser_forward")
            return VK_BROWSER_FORWARD;
        else if (key == "browser_refresh")
            return VK_BROWSER_REFRESH;
        else if (key == "browser_stop")
            return VK_BROWSER_STOP;
        else if (key == "browser_search")
            return VK_BROWSER_SEARCH;
        else if (key == "browser_favorites")
            return VK_BROWSER_FAVORITES;
        else if (key == "browser_home")
            return VK_BROWSER_HOME;
        else if (key == "volume_mute")
            return VK_VOLUME_MUTE;
        else if (key == "volume_down")
            return VK_VOLUME_DOWN;
        else if (key == "volume_up")
            return VK_VOLUME_UP;
        else if (key == "media_next")
            return VK_MEDIA_NEXT_TRACK;
        else if (key == "media_prev")
            return VK_MEDIA_PREV_TRACK;
        else if (key == "media_stop")
            return VK_MEDIA_STOP;
        else if (key == "media_play")
            return VK_MEDIA_PLAY_PAUSE;
        else if (key == "launch_mail")
            return VK_LAUNCH_MAIL;
        else if (key == "launch_media")
            return VK_LAUNCH_MEDIA_SELECT;
        else if (key == "launch_app1")
            return VK_LAUNCH_APP1;
        else if (key == "launch_app2")
            return VK_LAUNCH_APP2;
        else if (key == "oem_1")
            return VK_OEM_1;
        else if (key == "oem_plus")
            return VK_OEM_PLUS;
        else if (key == "oem_comma")
            return VK_OEM_COMMA;
        else if (key == "oem_minus")
            return VK_OEM_MINUS;
        else if (key == "oem_period")
            return VK_OEM_PERIOD;
        else if (key == "oem_2")
            return VK_OEM_2;
        else if (key == "oem_3")
            return VK_OEM_3;
        else if (key == "oem_4")
            return VK_OEM_4;
        else if (key == "oem_5")
            return VK_OEM_5;
        else if (key == "oem_6")
            return VK_OEM_6;
        else if (key == "oem_7")
            return VK_OEM_7;
        else if (key == "oem_8")
            return VK_OEM_8;
        else if (key == "oem_102")
            return VK_OEM_102;
        else if (key == "process")
            return VK_PROCESSKEY;
        else if (key == "packet")
            return VK_PACKET;
        else if (key == "attn")
            return VK_ATTN;
        else if (key == "crsel")
            return VK_CRSEL;
        else if (key == "exsel")
            return VK_EXSEL;
        else if (key == "ereof")
            return VK_EREOF;
        else if (key == "play")
            return VK_PLAY;
        else if (key == "zoom")
            return VK_ZOOM;
        else if (key == "noname")
            return VK_NONAME;
        else if (key == "pa1")
            return VK_PA1;
        else if (key == "oem_clear")
            return VK_OEM_CLEAR;
        else
        {
            if (key.length() == 1)
                return VkKeyScanA(key[0]);
            else
                return 0;
        }
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