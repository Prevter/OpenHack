#include "hide-triggers.hpp"
#include "../../menu/menu.hpp"

#ifndef OPENHACK_GEODE

#include <cocos2d.h>

#endif

namespace openhack::hacks {

    static std::vector<GameObject*> triggers;

    void HideTriggers::onInit() {
        // Set the default value
        config::setIfEmpty("hack.hide_triggers.enabled", false);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("hide_triggers.enabled", []() {
            bool enabled = !config::get<bool>("hack.hide_triggers.enabled");
            config::set("hack.hide_triggers.enabled", enabled);
        });
    }

    void HideTriggers::onDraw() {
        gui::checkbox("Auto Hide Triggers", "hack.hide_triggers.enabled");
        gui::tooltip("Automatically hides triggers in the editor when playtesting.");
        menu::keybinds::addMenuKeybind("hide_triggers.enabled", "Auto Hide Triggers", []() {
            bool enabled = !config::get<bool>("hack.hide_triggers.enabled", false);
            config::set("hack.hide_triggers.enabled", enabled);
        });
    }

    void HideTriggers::onPlaytest() {
        if (!config::get<bool>("hack.hide_triggers.enabled")) return;

        auto *editorLayer = LevelEditorLayer::get();
        if (!editorLayer) return;

        triggers.clear();
        auto objects = editorLayer->m_objects;
        for (int i = 0; i < objects->count(); i++) {
            auto obj = objects->objectAtIndex(i);
            auto* gameObject = reinterpret_cast<GameObject*>(obj);
            if (gameObject->m_objectType == GameObjectType::Modifier) {
                triggers.push_back(gameObject);
            }
        }
    }

    void HideTriggers::onStopPlaytest() {
        if (!config::get<bool>("hack.hide_triggers.enabled")) return;

        for (auto *trigger : triggers) {
            if (trigger) {
                trigger->setVisible(true);
            }
        }
        triggers.clear();
    }

    void HideTriggers::updateVisibility() {
        if (!config::get<bool>("hack.hide_triggers.enabled")) return;

        for (auto *trigger : triggers) {
            if (trigger) {
                trigger->setVisible(false);
            }
        }
    }

}