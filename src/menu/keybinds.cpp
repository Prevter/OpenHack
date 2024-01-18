#include "keybinds.h"
#include "gui.h"
#include "../config.h"
#include "../hook.h"

namespace keybinds
{
    std::vector<Keybind> keybinds;

    void init()
    {
    }

    void update()
    {
        for (auto &keybind : keybinds)
        {
            if (utils::is_key_pressed(keybind.keycode))
            {
                keybind.callback();
            }
        }
    }

    bool shortcut_button(const char *id, const char *label, std::function<void()> callback, float width = -1.f)
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
            if (keybinds::has_keybind(id))
            {
                if (ImGui::MenuItem("Remove keybind"))
                {
                    keybinds::remove_keybind(id);
                }
            }
            else
            {
                if (ImGui::MenuItem("Add keybind"))
                {
                    keybinds::add_keybind(keybinds::Keybind(id, label, callback));
                }
            }

            ImGui::EndPopup();
        }

        return pressed;
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
        //     "resetlevel", "Reset Level",
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

    void load_keybinds()
    {
    }
}