#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <map>
#include <vector>
#include <algorithm>
#include <shlobj.h>
#include <shellapi.h>

#include <gd.hpp>

namespace openhack::utils
{
    inline gd::cocos2d::CCPoint getMousePosition()
    {
        gd::cocos2d::CCDirector *director = gd::cocos2d::CCDirector::sharedDirector();
        auto *gl = gd::cocos2d::CCDirector::getOpenGLView(director);
        auto winSize = gd::cocos2d::CCDirector::getWinSize(director);
        auto frameSize = gl->getFrameSize();
        auto mouse = gl->getMousePosition() / frameSize;
        return gd::cocos2d::CCPoint(0, 0);
    }

    inline std::string getCurrentDirectory()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
    }

    inline std::string getSaveDirectory()
    {
        char buffer[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buffer);
        std::string path = std::string(buffer) + "\\GeometryDash";
        return path;
    }

    inline void openDirectory(const char *path)
    {
        ShellExecuteA(NULL, "open", path, NULL, NULL, SW_SHOWNORMAL);
    }

    inline std::string gameVersion = "";
    inline std::string getGameVersion()
    {
        if (gameVersion != "")
            return gameVersion;

        HMODULE module = GetModuleHandleA(NULL);
        auto dosHeader = (PIMAGE_DOS_HEADER)module;

        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            gameVersion = "Unknown";
            return gameVersion;
        }

        auto ntHeaders = (PIMAGE_NT_HEADERS)((uint8_t *)module + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
        {
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
        if (version != versionsMap.end())
        {
            gameVersion = version->second;
            return gameVersion;
        }

        // check if larger than latest version
        if (timestamp > versionsMap.rbegin()->first)
        {
            // split by dot
            std::vector<std::string> version;
            std::string version_str = versionsMap.rbegin()->second;
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
            
            // join by dot
            gameVersion = version[0] + "." + version[1];
            return gameVersion;
        }

        gameVersion = "Unknown";
        return gameVersion;
    }
}