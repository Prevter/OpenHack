#include "utils.hpp"
#include "pch.hpp"

namespace openhack::utils
{
    std::string getGameDirectory() noexcept
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
    }

    std::string getModSaveDirectory() noexcept
    {
        return getGameDirectory() + "/openhack";
    }

    std::string getModAssetsDirectory() noexcept
    {
        return getGameDirectory() + "/openhack";
    }

    std::string getModFontsDirectory() noexcept
    {
        return getModAssetsDirectory() + "/fonts";
    }

    std::string getModHacksDirectory() noexcept
    {
        return getModAssetsDirectory() + "/hacks";
    }
}