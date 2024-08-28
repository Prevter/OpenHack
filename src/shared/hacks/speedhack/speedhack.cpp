#include "speedhack.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void updateChannels() {
        FMOD::Channel *channel;
        FMOD::System *system = FMODAudioEngine::sharedEngine()->m_system;
        for (auto i = 0; i < 2; i++) {
            system->getChannel(126 + i, &channel);
            if (channel)
                SpeedHack::setVolume(channel);
        }
    }

    void SpeedHack::onInit() {
        // Set default values
        // We're disabling speedhack by default, so the game actually loads
        config::set("hack.speedhack.enabled", false);
        config::setIfEmpty("hack.speedhack.speed", 1.0f);
        config::setIfEmpty("hack.speedhack.music", false);

        // Create window
        menu::addWindow("Speedhack", [&]() {
            gui::widthF(0.5f);
            gui::inputFloat("##speed", "hack.speedhack.speed", 0.0f, FLT_MAX, "%.3fx");
            ImGui::SameLine(0, 2);
            gui::checkbox("Enabled", "hack.speedhack.enabled");
            menu::keybinds::addMenuKeybind("speedhack.enabled", "Speedhack", []() {
                config::set("hack.speedhack.enabled", !config::get<bool>("hack.speedhack.enabled"));
            });
            gui::width();

            gui::checkbox("Speedhack Audio", "hack.speedhack.music");
        });

        // Initialize keybinds
        menu::keybinds::setKeybindCallback("speedhack.enabled", []() {
            config::set("hack.speedhack.enabled", !config::get<bool>("hack.speedhack.enabled"));
        });
    }

    static bool s_lastSpeedhackState = false;

    void SpeedHack::update() {
        bool speedhack = config::get<bool>("hack.speedhack.enabled", false);
        bool toggledSpeedhack = s_lastSpeedhackState != speedhack;
        s_lastSpeedhackState = speedhack;

        bool sync = config::get<bool>("hack.speedhack.music", false);
        if ((speedhack || toggledSpeedhack) && sync)
            updateChannels();
    }

    bool SpeedHack::isCheating() {
        return config::get<bool>("hack.speedhack.enabled") && config::get<float>("hack.speedhack.speed") != 1.0f;
    }

    void SpeedHack::update(float *dt) {
        if (config::get<bool>("hack.speedhack.enabled")) {
            *dt *= config::get<float>("hack.speedhack.speed");
        }
    }

    void SpeedHack::setVolume(FMOD::Channel *channel) {
        float speed = 1.0f;

        if (config::get<bool>("hack.speedhack.enabled") && config::get<bool>("hack.speedhack.music"))
            speed = config::get<float>("hack.speedhack.speed");

        FMOD::Sound *sound;
        channel->getCurrentSound(&sound);
        float freq;
        sound->getDefaults(&freq, nullptr);
        channel->setFrequency(freq * speed);
    }

}