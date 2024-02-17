#include "openhack.hpp"

namespace openhack {
    void initialize() {
        utils::initRandom();

        L_INFO("Initializing OpenHack v" OPENHACK_VERSION "...");
        L_INFO("Game version: {}", utils::getGameVersion());

        try {
            gd::init();
            L_INFO("Initialized gd.hpp");
        }
        catch (const std::exception &e) {
            L_ERROR("Failed to initialize gd.hpp: {}", e.what());
        }

        config::load();
        config::setDefaults();

        menu::keybinds::load();

        hacks::initialize();
    }

    void deinitialize() {
        config::save();
    }
}