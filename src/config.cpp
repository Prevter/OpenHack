#include "config.h"
#include "hacks/hacks.h"
#include "menu/keybinds.h"

#define LOAD_KEY(name) \
    name = mod->getSavedValue<decltype(name)>(#name, name);

template <>
struct matjson::Serialize<nlohmann::json>
{
    static nlohmann::json from_json(matjson::Value const &value)
    {
        // load json from string
        std::string str = value.as_string();
        try
        {
            return nlohmann::json::parse(str);
        }
        catch (...)
        {
            L_WARN("Failed to parse json: {}", str);
            return nlohmann::json();
        }
    }

    static matjson::Value to_json(nlohmann::json const &value)
    {
        // save json as string
        try
        {
            return value.dump();
        }
        catch (...)
        {
            L_WARN("Failed to dump json: {}", value.dump());
            return matjson::Value();
        }
    }
};

namespace config
{
    bool always_reposition = true;
    std::string menu_hotkey = "tab";
    float menu_animation_length = 0.25f;
    float menu_size = 1.0f;
    float border_size = 0.0f;
    float window_rounding = 4.0f;
    int window_snap = 3;
    Color menu_color = {0.32f, 0.74f, 0.61f, 1.0f};
    Color frame_color = {0.11f, 0.11f, 0.11f, 0.54f};
    Color bg_color = {0.16f, 0.16f, 0.16f, 0.95f};
    Color text_color = {1.0f, 1.0f, 1.0f, 1.0f};
    Color disabled_color = {0.5f, 0.5f, 0.5f, 0.95f};
    bool text_color_rainbow = false;
    bool menu_color_rainbow = false;
    float menu_rainbow_speed = 0.15f, menu_rainbow_brightness = 0.65f;

    void load(std::string dir_name)
    {
        auto mod = geode::Mod::get();

        // "settings.json"
        LOAD_KEY(always_reposition);
        LOAD_KEY(menu_hotkey);
        LOAD_KEY(menu_animation_length);
        LOAD_KEY(menu_size);
        LOAD_KEY(border_size);
        LOAD_KEY(window_rounding);
        LOAD_KEY(window_snap);
        
        auto menu_color_hex = mod->getSavedValue<std::string>("menu_color", menu_color.to_hex());
        auto frame_color_hex = mod->getSavedValue<std::string>("frame_color", frame_color.to_hex());
        auto bg_color_hex = mod->getSavedValue<std::string>("bg_color", bg_color.to_hex());
        auto text_color_hex = mod->getSavedValue<std::string>("text_color", text_color.to_hex());
        auto disabled_color_hex = mod->getSavedValue<std::string>("disabled_color", disabled_color.to_hex());

        menu_color = Color::from_hex(menu_color_hex);
        frame_color = Color::from_hex(frame_color_hex);
        bg_color = Color::from_hex(bg_color_hex);
        text_color = Color::from_hex(text_color_hex);
        disabled_color = Color::from_hex(disabled_color_hex);

        LOAD_KEY(text_color_rainbow);
        LOAD_KEY(menu_color_rainbow);
        LOAD_KEY(menu_rainbow_speed);
        LOAD_KEY(menu_rainbow_brightness);

        // "windows.json"
        auto windows = mod->getSavedValue("window_positions", json());
        if (windows.is_object())
            globals::window_positions = windows;

        // load hacks settings from "hacks.json"
        auto hacks = mod->getSavedValue("hacks", json());
        if (hacks.is_object())
            hacks::load(&hacks);

        // load keybinds from "keybinds.json"
        auto keybinds = mod->getSavedValue("keybinds", json::array());
        if (keybinds.is_array())
            keybinds::load_keybinds(keybinds);

        // save config to recreate missing files
        save(dir_name);
    }

    void save(std::string dir_name)
    {
        auto mod = geode::Mod::get();

        // "settings.json"
        mod->setSavedValue("always_reposition", always_reposition);
        mod->setSavedValue("menu_hotkey", menu_hotkey);
        mod->setSavedValue("menu_animation_length", menu_animation_length);
        mod->setSavedValue("menu_size", menu_size);
        mod->setSavedValue("border_size", border_size);
        mod->setSavedValue("window_rounding", window_rounding);
        mod->setSavedValue("window_snap", window_snap);
        mod->setSavedValue("menu_color", menu_color);
        mod->setSavedValue("frame_color", frame_color);
        mod->setSavedValue("bg_color", bg_color);
        mod->setSavedValue("text_color", text_color);
        mod->setSavedValue("disabled_color", disabled_color);
        mod->setSavedValue("text_color_rainbow", text_color_rainbow);
        mod->setSavedValue("menu_color_rainbow", menu_color_rainbow);
        mod->setSavedValue("menu_rainbow_speed", menu_rainbow_speed);
        mod->setSavedValue("menu_rainbow_brightness", menu_rainbow_brightness);

        // "windows.json"
        mod->setSavedValue("window_positions", globals::window_positions);

        // "hacks.json"
        nlohmann::json j2;
        hacks::save(&j2);
        mod->setSavedValue("hacks", j2);

        // "keybinds.json"
        nlohmann::json j3;
        keybinds::save_keybinds(j3);
        mod->setSavedValue("keybinds", j3);
    }
}