#include "keybinds.h"
#include "gui.h"
#include "../config.h"
#include "../hook.h"
#include "../hacks/hacks.h"

namespace keybinds
{
    std::vector<Keybind> keybinds;

    void init()
    {
        for (auto &keybind : keybinds)
        {
            L_TRACE("Initializing keybind \"{}\"", keybind.id);
            auto id = keybind.id;

            // ask for hack to create its keybind
            auto component = hacks::find_component<hacks::Component>(id);
            if (component)
            {
                component->create_keybind(&keybind);
                L_TRACE("Initialized keybind for component \"{}\"", id);
                continue;
            }

            bool found = false;
            for (auto &hack : hacks::hacks)
            {
                if (hack->load_keybind(&keybind))
                {
                    L_TRACE("Initialized keybind for hack \"{}\"", id);
                    found = true;
                    break;
                }
            }

            if (!found)
                L_TRACE("Could not initialize keybind \"{}\"", id);
        }
    }

    void update()
    {
        for (auto &keybind : keybinds)
        {
            if (utils::is_key_pressed(keybind.keycode))
            {
                if (keybind.callback)
                    keybind.callback();
                else
                    L_WARN("Keybind \"{}\" has no callback", keybind.id);
            }
        }
    }

    bool shortcut_button(const char *id, const char *shortcut_name, const char *label, std::function<void()> callback, float width)
    {
        bool pressed = false;

        if (gui::ImButton(label, width))
        {
            callback();
            pressed = true;
        }

        auto popup_name = fmt::format("##popup_{}", id);
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(popup_name.c_str());
        }

        // draw popup menu
        if (ImGui::BeginPopup(popup_name.c_str()))
        {
            if (has_keybind(id))
            {
                if (ImGui::MenuItem("Remove keybind"))
                {
                    remove_keybind(id);
                }
            }
            else
            {
                if (ImGui::MenuItem("Add keybind"))
                {
                    add_keybind(Keybind(id, shortcut_name, callback));
                }
            }

            ImGui::EndPopup();
        }

        return pressed;
    }

    bool shortcut_toggle(const char *id, const char *shortcut_name, const char *label, bool *value, std::function<void()> callback, float width)
    {
        bool pressed = false;

        if (gui::ImToggleButton(label, value, nullptr, width))
        {
            if (callback)
                callback();
            pressed = true;
        }

        auto popup_name = fmt::format("##popup_{}", id);
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(popup_name.c_str());
        }

        // draw popup menu
        if (ImGui::BeginPopup(popup_name.c_str()))
        {
            if (has_keybind(id))
            {
                if (ImGui::MenuItem("Remove keybind"))
                {
                    remove_keybind(id);
                }
            }
            else
            {
                if (ImGui::MenuItem("Add keybind"))
                {
                    add_keybind(Keybind(
                        id, shortcut_name,
                        [value, callback]()
                        {
                            *value = !*value;
                            if (callback)
                                callback();
                        }));
                }
            }

            ImGui::EndPopup();
        }

        return pressed;
    }

    bool shortcut_button(const char *id, const char *label, std::function<void()> callback, float width)
    {
        return shortcut_button(id, label, label, callback, width);
    }

    bool shortcut_toggle(const char *id, const char *label, bool *value, std::function<void()> callback, float width)
    {
        return shortcut_toggle(id, label, label, value, callback, width);
    }

    void add_menu_keybind(const char *id, const char *label, std::function<void()> callback, float width)
    {
        auto popup_name = fmt::format("##popup_{}", id);
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup(popup_name.c_str());
        }

        // draw popup menu
        if (ImGui::BeginPopup(popup_name.c_str()))
        {
            if (has_keybind(id))
            {
                if (ImGui::MenuItem("Remove keybind"))
                {
                    remove_keybind(id);
                }
            }
            else
            {
                if (ImGui::MenuItem("Add keybind"))
                {
                    add_keybind(Keybind(id, label, callback));
                }
            }

            ImGui::EndPopup();
        }
    }

    void draw_menu()
    {
        gui::Begin("Keybinds");

        uint32_t original_open_key = utils::decode_key(config::menu_hotkey);
        uint32_t open_menu_key = original_open_key;
        gui::ImKeybind("Open menu", &open_menu_key, -1.f, 15, false);
        if (open_menu_key != original_open_key)
        {
            // make sure the key is a keyboard button (only WM_KEYDOWN and WM_SYSKEYDOWN are allowed)
            if (open_menu_key >= 0x01 && open_menu_key <= 0x06)
            {
                open_menu_key = original_open_key;
            }

            auto open_menu_key_name = utils::key_name(open_menu_key);
            config::menu_hotkey = open_menu_key_name;
            hook::set_menu_hotkey(open_menu_key_name);
        }

        ImGui::Separator();

        if (keybinds.empty())
            gui::ImText("Right click any hack and press\n\"Add keybind\" to set it.");

        for (auto &keybind : keybinds)
        {
            if (gui::ImKeybind(keybind.name.c_str(), &keybind.keycode))
            {
                remove_keybind(keybind.id);
            }
        }

        ImGui::Separator();

        // shortcut_button(
        //     "resetlevel", "Reset Level", "Reset Level",
        //     []()
        //     {
        //         // TODO:
        //         // find PlayLayer instance
        //         // call resetLevel() on it
        //     });

        gui::End();
    }

    void add_keybind(Keybind keybind)
    {
        // check if keybind already exists
        for (auto &keybind_ : keybinds)
        {
            if (keybind_.id == keybind.id)
            {
                return;
            }
        }

        keybinds.push_back(keybind);
    }

    void remove_keybind(std::string id)
    {
        for (auto it = keybinds.begin(); it != keybinds.end(); ++it)
        {
            if (it->id == id)
            {
                keybinds.erase(it);
                break;
            }
        }
    }

    void set_keybind(std::string id, uint32_t key)
    {
        for (auto &keybind : keybinds)
        {
            if (keybind.id == id)
            {
                keybind.keycode = key;
                break;
            }
        }
    }

    bool has_keybind(std::string id)
    {
        for (auto &keybind : keybinds)
        {
            if (keybind.id == id)
            {
                return true;
            }
        }
        return false;
    }

    void load_keybinds(nlohmann::json &j)
    {
        for (auto &keybind : j)
        {
            auto id = keybind["id"].get<std::string>();
            auto key = keybind["key"].get<uint32_t>();
            auto name = keybind["name"].get<std::string>();
            Keybind keybind(id, name, nullptr, key);
            add_keybind(keybind);
        }
    }

    void save_keybinds(nlohmann::json &j)
    {
        for (auto &keybind : keybinds)
        {
            nlohmann::json keybind_json;
            keybind_json["id"] = keybind.id;
            keybind_json["key"] = keybind.keycode;
            keybind_json["name"] = keybind.name;
            j.push_back(keybind_json);
        }
    }
}