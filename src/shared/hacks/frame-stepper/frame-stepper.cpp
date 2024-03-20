#include "frame-stepper.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void FrameStepper::onInit() {
        // Set the default value
        config::setIfEmpty("hack.framestep.enabled", false);
        config::setIfEmpty("hack.framestep.step_key", "C");

        // Initialize keybind
        menu::keybinds::setKeybindCallback("framestep.enabled", []() {
            bool enabled = !config::get<bool>("hack.framestep.enabled");
            config::set("hack.framestep.enabled", enabled);
        });
    }

    void FrameStepper::onDraw() {
        gui::callback([](){
            gui::tooltip("Allows you to step through the game frame by frame");
            menu::keybinds::addMenuKeybind("framestep.enabled", "Frame Stepper", [](){
                bool enabled = !config::get<bool>("hack.framestep.enabled");
                config::set("hack.framestep.enabled", enabled);
            });
        });
        gui::toggleSetting("Frame Stepper", "hack.framestep.enabled", [](){
            gui::width(90);
            gui::keybind("Step key", "hack.framestep.step_key");
            gui::width();
        }, ImVec2(0, 0), 150);
    }

    bool FrameStepper::isCheating() {
        return config::get<bool>("hack.framestep.enabled", false);
    }

    void FrameStepper::gameUpdate(float* dt) {
        if (!config::get<bool>("hack.framestep.enabled", false)) return;

        auto step = 240.f;
        if (config::get<bool>("hack.display.tps_bypass")) {
            step = config::get<float>("hack.display.tps", 240);
        }

        if (utils::isKeyPressed(config::get<std::string>("hack.framestep.step_key"))) {
            *dt = 1.0f / step;
        } else {
            *dt = 0.0f;
        }
    }

}