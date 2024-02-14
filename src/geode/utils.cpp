#include "utils.hpp"
#include <Geode/Geode.hpp>

namespace openhack::utils {
    std::string getModSaveDirectory() noexcept {
        return geode::Mod::get()->getSaveDir().string();
    }

    std::string getModAssetsDirectory() noexcept {
        return geode::Mod::get()->getResourcesDir().string();
    }

    std::string getModFontsDirectory() noexcept {
        // geode packs all files into a single directory
        return getModAssetsDirectory();
    }

    std::string getModHacksDirectory() noexcept {
        return getModAssetsDirectory();
    }
}