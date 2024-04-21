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
        auto respawnDelayHook = sinaps::match(
                "E9????F30F1005^????68????C683",
                utils::bytesToHex(utils::getBytes((uintptr_t)&delay)));
        auto customBypass = sinaps::match("84C0^7410F30F10", "EB");

        if (respawnDelayHook.isErr()) {
            L_WARN("Failed to find signature for RespawnDelay: {}", respawnDelayHook.err());
            return;
        } else if (customBypass.isErr()) {
            L_WARN("Failed to find signature for RespawnDelay: {}", customBypass.err());
            return;
        }

        std::vector<sinaps::patch_t> opcodes = { respawnDelayHook.val(), customBypass.val() };

        if (openhack::debugMode) {
            L_INFO("Respawn Delay patch addresses:");
            L_INFO("  - RespawnDelay: 0x{:X}", (uintptr_t) respawnDelayHook.val().address);
            L_INFO("  - CustomBypass: 0x{:X}", (uintptr_t) customBypass.val().address);
        }

        s_respawnDelay = new ToggleComponent("", "", opcodes);
        togglePatch();

        // Initialize keybind
        menu::keybinds::setKeybindCallback("respawn_delay.enabled", []() {
            bool enabled = !config::get<bool>("hack.respawn_delay.enabled");
            config::set("hack.respawn_delay.enabled", enabled);
            togglePatch();
        });
    }

    void RespawnDelay::onDraw() {
        gui::callback([](){
            gui::tooltip("Allows you to change the default respawn time");
            menu::keybinds::addMenuKeybind("respawn_delay.enabled", "Respawn Delay", [](){
                bool enabled = !config::get<bool>("hack.respawn_delay.enabled");
                config::set("hack.respawn_delay.enabled", enabled);
                togglePatch();
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
            togglePatch();
        }
    }

}