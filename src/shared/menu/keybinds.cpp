#include "keybinds.hpp"
#include "../config.hpp"
#include "menu.hpp"

namespace openhack::menu::keybinds {
    static std::vector<Keybind> keybinds;

    void addKeybind(const Keybind &keybind) {
        keybinds.push_back(keybind);
    }

    void removeKeybind(const std::string &id) {
        for (auto it = keybinds.begin(); it != keybinds.end(); ++it) {
            if (it->id == id) {
                keybinds.erase(it);
                return;
            }
        }
    }

    Keybind getKeybind(const std::string &id) {
        for (auto &keybind: keybinds) {
            if (keybind.id == id) {
                return keybind;
            }
        }
        return {"", "", 0};
    }

    bool hasKeybind(const std::string &id) {
        return std::any_of(keybinds.begin(), keybinds.end(), [&](const Keybind &keybind) {
            return keybind.id == id;
        });
    }

    std::vector<Keybind> getKeybinds() {
        return keybinds;
    }

    void setKeybindCallback(const std::string &id, const std::function<void()> &callback) {
        for (auto &keybind: keybinds) {
            if (keybind.id == id) {
                keybind.callback = callback;
                return;
            }
        }
    }

    void load() {
        keybinds = config::get("keybinds", std::vector<Keybind>());

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

            if (keybinds.empty())
                gui::text("Right click any hack and\npress \"Add keybind\" to set it.");

            for (auto &keybind: keybinds) {
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
        if (config::get<bool>("keybinds.ingame") && !gd::PlayLayer::get())
            return;

        for (auto &keybind: keybinds) {
            if (utils::isKeyPressed(keybind.keycode) && keybind.callback) {
                keybind.callback();
                save();
            }
        }
    }

    void save() {
        config::set("keybinds", keybinds);
        config::save();
    }

    void addMenuKeybind(const std::string &id, const std::string &label, const std::function<void()> &callback) {
        auto popupName = "##popup_" + id;
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
                    addKeybind({label, id, 0, callback});
                    save();
                }
            }
            ImGui::EndPopup();
        }
    }
}