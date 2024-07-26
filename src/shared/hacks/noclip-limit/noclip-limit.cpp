#include "noclip-limit.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    bool isDead = false;
    bool lastFrameDead = false;
    bool isAntiCheatDeath = false;
    GameObject* antiCheatObject = nullptr;
    uint32_t deathCount = 0;

    void NoclipLimit::onInit() {
        // Set the default value
        config::setIfEmpty("hack.noclip_limit.enabled", false);
        config::setIfEmpty("hack.noclip_limit.deaths", 10.0f);
        config::setIfEmpty("hack.noclip_limit.killOnDeaths", false);
        config::setIfEmpty("hack.noclip_limit.accuracy", 75.0f);
        config::setIfEmpty("hack.noclip_limit.killOnAccuracy", true);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("noclip_limit.enabled", []() {
            bool enabled = !config::get<bool>("hack.noclip_limit.enabled");
            config::set("hack.noclip_limit.enabled", enabled);
        });
    }

    void NoclipLimit::onDraw() {
        gui::callback([](){
            gui::tooltip("Automatically kills the player when you reach the death limit or accuracy limit");
            menu::keybinds::addMenuKeybind("noclip_limit.enabled", "Noclip Limit", [](){
                bool enabled = !config::get<bool>("hack.noclip_limit.enabled");
                config::set("hack.noclip_limit.enabled", enabled);
            });
        });
        gui::toggleSetting("Noclip Limit", "hack.noclip_limit.enabled", []() {
            gui::width(150);
            gui::checkbox("Deaths Limit", "hack.noclip_limit.killOnDeaths");
            gui::tooltip("If enabled, the player will be killed when they reach the death limit.");
            gui::inputFloat("Deaths", "hack.noclip_limit.deaths", 1, FLT_MAX, "%.0f");
            gui::tooltip("The amount of deaths before the player is killed.");

            gui::checkbox("Accuracy Limit", "hack.noclip_limit.killOnAccuracy");
            gui::tooltip("If enabled, the player will be killed when accuracy drops lower than the accuracy limit");
            gui::inputFloat("Accuracy", "hack.noclip_limit.accuracy", 0, 100, "%.2f");
            gui::tooltip("Minimum allowed accuracy, before the player is killed");
            gui::width();
        }, ImVec2(0, 0), 150);
    }

    bool shouldDie() {
        auto killOnDeaths = config::get<bool>("hack.noclip_limit.killOnDeaths", false);
        auto killOnAccuracy = config::get<bool>("hack.noclip_limit.killOnAccuracy", false);

        bool result = false;

        if (killOnDeaths) {
            auto deathLimit = (int32_t)config::get<float>("hack.noclip_limit.deaths", 0);
            auto deaths = config::getGlobal<int32_t>("noclipDeath", 0);
            result = deaths >= deathLimit;
        }

        if (killOnAccuracy) {
            auto accLimit = config::get<float>("hack.noclip_limit.accuracy", 75.0f);
            auto accuracy = config::getGlobal<float>("noclipAcc", 100.0f);
            result |= accuracy < accLimit;
        }

        return result;
    }

    void NoclipLimit::destroyPlayer(GameObject* object) {
        isAntiCheatDeath = false;

        auto frames = config::getGlobal("frame", 0);
        if (!antiCheatObject && frames < 5) {
            isAntiCheatDeath = true;
            antiCheatObject = object;
            return;
        }

        if (antiCheatObject == object) {
            isAntiCheatDeath = true;
            return;
        }

        if (!config::get<bool>("hack.noclip_limit.enabled", false)) return;

        if (shouldDie()) {
            hacks::getHack("level.noclip")->applyPatch(false); // disable noclip
        }
    }

    void NoclipLimit::postDestroyPlayer() {
        if (isAntiCheatDeath) return;
        isDead = true;

        if (!config::get<bool>("hack.noclip_limit.enabled", false)) return;

        if (shouldDie()) {
            hacks::getHack("level.noclip")->applyPatch(); // restore original setting
        }
    }

    void NoclipLimit::resetLevel() {
        isDead = false;
        lastFrameDead = false;
        deathCount = 0;
        antiCheatObject = nullptr;
        config::setGlobal("noclipAcc", 100.0f);
        config::setGlobal("noclipDeath", 0);
    }

    void NoclipLimit::processCommands() {
        auto* playlayer = PlayLayer::get();
        if (!playlayer || playlayer->m_hasCompletedLevel || playlayer->m_player1->m_isDead) return;

        if (isDead) {
            deathCount++;
            if (!lastFrameDead) {
                auto deaths = config::getGlobal("noclipDeath", 0);
                config::setGlobal("noclipDeath", deaths + 1);
            }
        }

        lastFrameDead = isDead;
        isDead = false;

        // calculate accuracy
        auto frame = config::getGlobal("frame", 0);
        if (frame > 0) {
            float acc = (float) (frame - deathCount) / (float) frame * 100.f;
            config::setGlobal("noclipAcc", acc);
        }
    }

}