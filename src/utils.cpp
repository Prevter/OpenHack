#include "utils.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <map>

namespace utils
{
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
            game_version = (char *)(version->second + "+").c_str();
            return game_version;
        }

        game_version = "unknown";
        return game_version;
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
}