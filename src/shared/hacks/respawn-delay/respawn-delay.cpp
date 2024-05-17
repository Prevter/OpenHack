#include "respawn-delay.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    float RespawnDelay::delay = 0.5f;
    static ToggleComponent* s_respawnDelay = nullptr;

    void toggleDelayPatch() {
        if (!s_respawnDelay) return;
        bool enabled = config::get<bool>("hack.respawn_delay.enabled");
        s_respawnDelay->applyPatch(enabled);
    }

    void RespawnDelay::onInit() {
        // Set default values
        config::setIfEmpty("hack.respawn_delay.enabled", false);
        config::setIfEmpty("hack.respawn_delay.delay", 0.5f);
        delay = config::get<float>("hack.respawn_delay.delay");

        // Initialize toggle
        std::vector<gd::sigscan::Opcode> opcodes = gd::sigscan::match(
                "E9????F30F1005^????68????C683",
                utils::bytesToHex(utils::getBytes((uintptr_t)&delay)));

        std::map<std::string, std::string> bypasses = {
                {"F30F108B????0F2FC8^76", "EB"}, // respawnTime = this->m_gameState.m_unk2c8;
                {"84C0^7410F30F10", "EB"}, // respawnTime = 0.5f;
                {"F30F104424180F2FC1^77", "EB"} // respawnTime = 1.4f;
        };

        std::vector<gd::sigscan::Opcode> customBypass;
        for (auto& [pattern, mask] : bypasses) {
            auto opcode = gd::sigscan::match(pattern, mask);
            if (!opcode.empty()) {
                customBypass.push_back(opcode[0]);
            }
        }

        if (openhack::debugMode) {
            L_INFO("Respawn Delay patch addresses:");
            for (auto& opcode : opcodes) {
                L_TRACE("[Set custom time] 0x{:x}", opcode.address);
            }
            for (auto& opcode : customBypass) {
                L_TRACE("[Bypass 0.5s restart] 0x{:x}", opcode.address);
            }
        }

        if (opcodes.empty() || customBypass.empty()) {
            L_WARN("Failed to find signature for RespawnDelay");
            return;
        }

        // Merge opcodes and customBypass
        opcodes.insert(opcodes.end(), customBypass.begin(), customBypass.end());
        s_respawnDelay = new ToggleComponent("", "", opcodes);
        toggleDelayPatch();

        // Initialize keybind
        menu::keybinds::setKeybindCallback("respawn_delay.enabled", []() {
            bool enabled = !config::get<bool>("hack.respawn_delay.enabled");
            config::set("hack.respawn_delay.enabled", enabled);
            toggleDelayPatch();
        });
    }

    void RespawnDelay::onDraw() {
        gui::callback([](){
            gui::tooltip("Allows you to change the default respawn time");
            menu::keybinds::addMenuKeybind("respawn_delay.enabled", "Respawn Delay", [](){
                bool enabled = !config::get<bool>("hack.respawn_delay.enabled");
                config::set("hack.respawn_delay.enabled", enabled);
                toggleDelayPatch();
            });
        });
        if (gui::toggleSetting("Respawn Delay", "hack.respawn_delay.enabled", []() {
            gui::width(100);
            if (gui::inputFloat("Delay", "hack.respawn_delay.delay", 0.0f, FLT_MAX, "%.3f sec.")) {
                delay = config::get<float>("hack.respawn_delay.delay");
            }
            gui::tooltip("The delay before respawning");
            gui::width();
        }, ImVec2(0, 0))) {
            toggleDelayPatch();
        }
    }

}