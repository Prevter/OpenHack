#include "respawn-delay.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    float RespawnDelay::delay = 0.5f;
    static ToggleComponent* s_respawnDelay = nullptr;

    inline void togglePatch() {
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
                "F30F1005^????68????C6",
                utils::bytesToHex(utils::getBytes((uintptr_t)&delay)));
        auto customBypass = gd::sigscan::match("84C0^7410F30F10", "EB");

        if (opcodes.empty() || customBypass.empty()) {
            L_WARN("Failed to find signature for RespawnDelay");
            return;
        }

        opcodes.push_back(customBypass[0]);
        s_respawnDelay = new ToggleComponent("", "", opcodes);
        togglePatch();
    }

    void RespawnDelay::onDraw() {
        gui::callback([](){
            gui::tooltip("Allows you to change the default respawn time");
        });
        if (gui::toggleSetting("Respawn Delay", "hack.respawn_delay.enabled", []() {
            gui::width(100);
            if (gui::inputFloat("Delay", "hack.respawn_delay.delay", 0.0f, FLT_MAX, "%.3f sec.")) {
                delay = config::get<float>("hack.respawn_delay.delay");
            }
            gui::tooltip("The delay before respawning");
            gui::width();
        }, ImVec2(0, 0))) {
            togglePatch();
        }
    }

}