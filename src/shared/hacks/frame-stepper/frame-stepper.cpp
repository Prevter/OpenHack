#include "frame-stepper.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    static double s_holdDelayTimer = 0;
    static uint32_t s_holdAdvanceTimer = 0;

    void FrameStepper::onInit() {
        // Set the default value
        config::setIfEmpty("hack.framestep.enabled", false);
        config::setIfEmpty("hack.framestep.step_key", "C");
        config::setIfEmpty("hack.framestep.hold", false);
        config::setIfEmpty("hack.framestep.hold_delay", 0.25f);
        config::setIfEmpty("hack.framestep.hold_speed", 5.0f);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("framestep.enabled", []() {
            bool enabled = !config::get<bool>("hack.framestep.enabled");
            config::set("hack.framestep.enabled", enabled);
        });
    }

    void FrameStepper::onDraw() {
        gui::callback([]() {
            gui::tooltip("Allows you to step through the game frame by frame");
            menu::keybinds::addMenuKeybind("framestep.enabled", "Frame Stepper", []() {
                bool enabled = !config::get<bool>("hack.framestep.enabled");
                config::set("hack.framestep.enabled", enabled);
            });
        });
        gui::toggleSetting("Frame Stepper", "hack.framestep.enabled", []() {
            gui::width(90);
            gui::keybind("Step key", "hack.framestep.step_key");
            gui::checkbox("Hold to advance", "hack.framestep.hold");
            gui::tooltip("Hold the step key to advance the game frame by frame");
            gui::inputFloat("Hold delay", "hack.framestep.hold_delay", 0.f, FLT_MAX, "%.2f");
            gui::tooltip("How long to wait before activating the hold feature");
            gui::inputFloat("Hold speed", "hack.framestep.hold_speed", 0.f, FLT_MAX, "%.0f");
            gui::tooltip("How many frames to skip when holding the step key. (0 = play at normal speed)");
            gui::width();
        }, ImVec2(0, 0), 150);
    }

    bool FrameStepper::isCheating() {
        return config::get<bool>("hack.framestep.enabled", false);
    }

    void FrameStepper::gameUpdate(float *dt) {
        if (!config::get<bool>("hack.framestep.enabled", false)) return;

        auto step = 240.f;
        if (config::get<bool>("hack.display.physics_bypass")) {
            step = config::get<float>("hack.display.pfps", 240);
        }

        auto holdTimer = static_cast<uint32_t>(config::get<float>("hack.framestep.hold_speed", 5));

        bool shouldStep = false;
        if (config::get<bool>("hack.framestep.hold")) {
            s_holdDelayTimer += *dt;
            auto stepKey = config::get<std::string>("hack.framestep.step_key");
            if (utils::isKeyPressed(stepKey)) {
                s_holdDelayTimer = 0;
            }

            shouldStep = utils::isKeyDown(stepKey);
            auto delay = config::get<float>("hack.framestep.hold_delay", 0.1f);
            if (shouldStep) {
                s_holdAdvanceTimer++;
                if (s_holdAdvanceTimer >= holdTimer) {
                    s_holdAdvanceTimer = 0;
                    shouldStep = s_holdDelayTimer >= delay;
                } else {
                    shouldStep = s_holdDelayTimer == 0;
                }
            } else {
                s_holdAdvanceTimer = 0;
            }
        } else {
            shouldStep = utils::isKeyPressed(config::get<std::string>("hack.framestep.step_key"));
        }

        if (shouldStep) {
            *dt = 1.0f / step;
        } else {
            *dt = 0.0f;
        }
    }

}