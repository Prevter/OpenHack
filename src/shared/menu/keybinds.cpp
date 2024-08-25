#include "keybinds.hpp"
#include "../config.hpp"
#include "menu.hpp"

namespace openhack::menu::keybinds {
    std::vector<Keybind>& getKeybinds() {
        static std::vector<Keybind> keybinds;
        return keybinds;
    }

    void addKeybind(const Keybind &keybind) {
        getKeybinds().push_back(keybind);
    }

    void removeKeybind(std::string_view id) {
        for (auto it = getKeybinds().begin(); it != getKeybinds().end(); ++it) {
            if (it->id == id) {
                getKeybinds().erase(it);
                return;
            }
        }
    }

    Keybind getKeybind(std::string_view id) {
        for (auto &keybind: getKeybinds()) {
            if (keybind.id == id) {
                return keybind;
            }
        }
        return {"", "", 0};
    }

    bool hasKeybind(std::string_view id) {
        return std::any_of(getKeybinds().begin(), getKeybinds().end(), [&](const Keybind &keybind) {
            return keybind.id == id;
        });
    }

    void setKeybindCallback(std::string_view id, const std::function<void()> &callback) {
        for (auto &keybind: getKeybinds()) {
            if (keybind.id == id) {
                keybind.callback = callback;
                return;
            }
        }
    }

    void load() {
        getKeybinds() = config::get("keybinds", std::vector<Keybind>());

        // Create the keybinds window
        menu::addWindow("Keybinds", [&]() {
            gui::keybind("Open Menu", "menu.toggleKey");
            // Make sure user can't set open menu keybind to LMB,
            // because it won't be possible to use the menu anymore
            if (config::get<std::string>("menu.toggleKey") == "LMB") {
                config::set("menu.toggleKey", "Tab");
            }

            gui::checkbox("In-game only", "keybinds.ingame");
            gui::tooltip("Only allow keybinds to be triggered when you're inside a level.\nUseful, to avoid triggering keybinds while searching for levels.");

            if (getKeybinds().empty())
                gui::text("Right click any hack and\npress \"Add keybind\" to set it.");

            for (auto &keybind: getKeybinds()) {
                uint32_t originalKeycode = keybind.keycode;
                if (gui::keybind(keybind.name.c_str(), &keybind.keycode, true)) {
                    removeKeybind(keybind.id);
                    save();
                }

                if (originalKeycode != keybind.keycode) {
                    save();
                }
            }
        });
    }

    void update() {
        if (config::get<bool>("keybinds.ingame") && !PlayLayer::get())
            return;

        for (auto &keybind: getKeybinds()) {
            if (utils::isKeyPressed(keybind.keycode) && keybind.callback) {
                keybind.callback();
                save();
            }
        }
    }

    void save() {
        config::set("keybinds", getKeybinds());
        config::save();
    }

    void addMenuKeybind(std::string_view id, std::string_view label, const std::function<void()> &callback) {
        auto popupName = fmt::format("##popup_{}", id);
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup(popupName.c_str());
        }

        if (ImGui::BeginPopup(popupName.c_str())) {
            if (hasKeybind(id)) {
                if (ImGui::MenuItem("Remove keybind")) {
                    removeKeybind(id);
                    save();
                }
            } else {
                if (ImGui::MenuItem("Add keybind")) {
                    addKeybind({std::string(label), std::string(id), 0, callback});
                    save();
                }
            }
            ImGui::EndPopup();
        }
    }
}