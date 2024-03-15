#include "random-seed.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    static uint32_t s_editedSeed = 0;
    static uintptr_t s_seedAddr = 0;

    uint32_t& RandomSeed::getCurrentSeed() {
        if (s_seedAddr == 0) {
            L_WARN("Seed address is not found");
            return s_editedSeed;
        }
        return *reinterpret_cast<uint32_t*>(s_seedAddr);
    }

    void RandomSeed::onInit() {
        // Set the default value
        config::setIfEmpty("hack.random_seed.enabled", false);
        config::setIfEmpty("hack.random_seed.seed", 0);
        config::setIfEmpty("hack.random_seed.freeze", false);

        // Find the seed address from "mov [DWORD PTR ds:?], eax"
        uintptr_t movToSeedAddr = gd::sigscan::findPattern("A3^???00C783??000000000000C6");
        if (movToSeedAddr != 0) {
            L_TRACE("Found mov to seed address at 0x{:x}", movToSeedAddr);
            s_seedAddr = *reinterpret_cast<uint32_t*>(movToSeedAddr);
            L_TRACE("Found seed address at 0x{:x}", s_seedAddr);
        } else {
            L_WARN("Failed to find seed address");
            return;
        }
    }

    void RandomSeed::onDraw() {
        gui::toggleSetting("Random Seed", "hack.random_seed.enabled", []() {
            gui::width(80);
            gui::inputFloat("Seed", "hack.random_seed.seed", 0, FLT_MAX, "%.0f");
            gui::width();
            gui::tooltip("Set the random seed every time the level is restarted.");
            gui::checkbox("Freeze", "hack.random_seed.freeze");
            gui::tooltip("Lock the random seed to the value set above.");

            ImGui::Separator();
            gui::width(80);
            auto fltEditedSeed = static_cast<float>(s_editedSeed);
            gui::inputFloat("##applySeed", &fltEditedSeed, 0, FLT_MAX, "%.0f");
            s_editedSeed = static_cast<uint32_t>(fltEditedSeed);
            gui::width();
            ImGui::SameLine(0, 2);
            if (gui::button("Apply")) {
                getCurrentSeed() = s_editedSeed;
            }
            gui::tooltip("Set the random seed value.");
            gui::text("Current seed: %d", getCurrentSeed());
        }, ImVec2(0, 0), 150);
    }

    void RandomSeed::update() {
        if (!isCheating()) return;

        if (config::get<bool>("hack.random_seed.freeze", false)) {
            getCurrentSeed() = config::get<uint32_t>("hack.random_seed.seed", 0);
        }
    }

    bool RandomSeed::isCheating() {
        return config::get<bool>("hack.random_seed.enabled", false);
    }

    void RandomSeed::resetLevel() {
        if (config::get<bool>("hack.random_seed.enabled", false)) {
            getCurrentSeed() = config::get<uint32_t>("hack.random_seed.seed", 0);
        }
    }
}