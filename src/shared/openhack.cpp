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
        L_INFO("Loaded config");

        menu::keybinds::load();
        L_INFO("Loaded keybinds");

        hacks::initialize();
        L_INFO("Initialized hacks");
    }

    void deinitialize() {
        config::save();
    }
}