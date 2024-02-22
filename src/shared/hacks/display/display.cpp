#include "display.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void Display::onInit() {
        config::setGlobal("physicsTickAddr", gd::sigscan::findPattern("8988883B"));

        // Create window
        menu::addWindow("Display", [&]() {
            bool needRefresh = false;

            gui::widthF(0.35f);
            needRefresh |= gui::inputFloat("##fps", "hack.display.fps", 10.0f, FLT_MAX, "%.0f FPS");
            ImGui::SameLine(0, 2);
            needRefresh |= gui::checkbox("Unlock FPS", "hack.display.unlock_fps");
            menu::keybinds::addMenuKeybind("display.unlock_fps", "Unlock FPS", []() {
                config::set("hack.display.unlock_fps", !config::get<bool>("hack.display.unlock_fps"));
                refreshRate();
                refreshPhysics();
            });
            gui::width();

            gui::widthF(0.35f);
            needRefresh |= gui::inputFloat("##pfps", "hack.display.pfps", 10.0f, FLT_MAX, "%.0f TPS");
            ImGui::SameLine(0, 2);
            needRefresh |= gui::checkbox("Physics Bypass", "hack.display.physics_bypass");
            menu::keybinds::addMenuKeybind("display.physics_bypass", "Physics Bypass", []() {
                config::set("hack.display.physics_bypass", !config::get<bool>("hack.display.physics_bypass"));
                refreshRate();
                refreshPhysics();
            });
            gui::width();

            needRefresh |= gui::checkbox("Vertical Sync", "hack.display.vsync");
            menu::keybinds::addMenuKeybind("display.vsync", "Vertical Sync", []() {
                config::set("hack.display.vsync", !config::get<bool>("hack.display.vsync"));
                refreshRate();
                refreshPhysics();
            });
            // gui::checkbox("Fullscreen", "hack.display.fullscreen");
            // gui::checkbox("Borderless", "hack.display.borderless");

            if (needRefresh) {
                refreshRate();
                refreshPhysics();
            }
        });

        // Initialize keybinds
        menu::keybinds::setKeybindCallback("display.unlock_fps", []() {
            config::set("hack.display.unlock_fps", !config::get<bool>("hack.display.unlock_fps"));
            refreshRate();
            refreshPhysics();
        });

        menu::keybinds::setKeybindCallback("display.physics_bypass", []() {
            config::set("hack.display.physics_bypass", !config::get<bool>("hack.display.physics_bypass"));
            refreshRate();
            refreshPhysics();
        });
    }

    void Display::onLateInit() {
        // Set default values
        auto *manager = gd::GameManager::sharedState();
        config::setIfEmpty("hack.display.fps", manager->m_customFPSTarget());
        config::setIfEmpty("hack.display.unlock_fps", manager->getGameVariable("0116"));
        config::setIfEmpty("hack.display.pfps", 240.0f);
        config::setIfEmpty("hack.display.physics_bypass", false);
        config::setIfEmpty("hack.display.vsync", manager->getGameVariable("0030"));
        config::setIfEmpty("hack.display.fullscreen", false);
        config::setIfEmpty("hack.display.borderless", false);
    }

    void Display::refreshRate() {
        auto *application = gd::cocos2d::CCApplication::sharedApplication();
        auto *manager = gd::GameManager::sharedState();

        // Get settings
        bool vsync = config::get<bool>("hack.display.vsync");
        bool unlockFps = config::get<bool>("hack.display.unlock_fps");

        // Save settings
        manager->setGameVariable("0030", vsync);
        manager->setGameVariable("0116", unlockFps);

        // Set Vertical Sync
        application->toggleVerticalSync(vsync);
        if (vsync)
            return;

        // Set FPS
        if (unlockFps) {
            auto fps = config::get<float>("hack.display.fps");
            manager->m_customFPSTarget(fps);
        }
        manager->updateCustomFPS();
    }

    void Display::refreshPhysics() {
        bool bypass = config::get<bool>("hack.display.physics_bypass");
        float pfps = bypass ? config::get<float>("hack.display.pfps") : 240.0f;

        utils::writeMemory<float>(config::getGlobal<uintptr_t>("physicsTickAddr"), 1.0 / pfps);
        if (pfps == 240.0f)
            return;

        auto *playLayer = gd::PlayLayer::get();
        if (playLayer && playLayer->m_level()->m_timestamp() > 0) {
            float startTimestamp = config::getGlobal<float>("startTimestamp");
            float timeMultiplier = pfps / 240.0f;
            float stepsMultiplier = (startTimestamp * timeMultiplier) / playLayer->m_level()->m_timestamp();
            float originalValue = playLayer->m_gameState().m_stepSpeed();
            playLayer->m_level()->m_timestamp(startTimestamp * timeMultiplier);
            playLayer->m_gameState().m_stepSpeed(originalValue * stepsMultiplier);
        }
    }

    bool Display::isCheating() {
        return (config::get<bool>("hack.display.physics_bypass") &&
               config::get<float>("hack.display.pfps") != 240.0f);
    }

    void Display::playLayerInit(gd::GJGameLevel *level) {
        config::setGlobal("startTimestamp", level->m_timestamp());
    }

    void Display::playLayerReset() {
        refreshPhysics();
    }

}