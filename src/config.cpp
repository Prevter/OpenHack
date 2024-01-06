#include "config.h"
#include "hacks/hacks.h"

#define LOAD_KEY(name)     \
    if (j.contains(#name)) \
        name = j[#name].get<decltype(name)>();

#define LOAD_COLOR(name)                                 \
    if (j.contains(#name))                               \
    {                                                    \
        auto color = j[#name];                           \
        name = {color[0], color[1], color[2], color[3]}; \
    }

namespace config
{
    bool reposition_windows = true;
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
        // create directory if it doesn't exist
        std::filesystem::create_directory(dir_name);

        // check if config file exists
        if (std::filesystem::exists(dir_name + "\\settings.json"))
        {
            // load config from "settings.json"
            std::ifstream file(dir_name + "\\settings.json");
            if (!file.is_open())
            {
                L_ERROR("Failed to open config file!");
                return;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();

            std::string json = buffer.str();

            // parse json
            try
            {
                nlohmann::json j = nlohmann::json::parse(json);
                LOAD_KEY(reposition_windows);
                LOAD_KEY(menu_hotkey);
                LOAD_KEY(menu_animation_length);
                LOAD_KEY(menu_size);
                LOAD_KEY(border_size);
                LOAD_KEY(window_rounding);
                LOAD_KEY(window_snap);
                LOAD_COLOR(menu_color);
                LOAD_COLOR(frame_color);
                LOAD_COLOR(bg_color);
                LOAD_COLOR(text_color);
                LOAD_COLOR(disabled_color);
                LOAD_KEY(text_color_rainbow);
                LOAD_KEY(menu_color_rainbow);
                LOAD_KEY(menu_rainbow_speed);
                LOAD_KEY(menu_rainbow_brightness);
            }
            catch (const std::exception &e)
            {
                L_ERROR("Failed to parse config file: {}", e.what());
            }
        }

        // load window positions from "windows.json"
        if (std::filesystem::exists(dir_name + "\\windows.json"))
        {
            std::ifstream file2(dir_name + "\\windows.json");
            if (!file2.is_open())
            {
                L_ERROR("Failed to open window positions file!");
                return;
            }

            std::stringstream buffer;
            buffer << file2.rdbuf();
            file2.close();

            std::string json = buffer.str();

            // parse json
            try
            {
                globals::window_positions = nlohmann::json::parse(json);
            }
            catch (const std::exception &e)
            {
                L_ERROR("Failed to parse window positions file: {}", e.what());
            }
        }

        // load hacks settings from "hacks.json"
        if (std::filesystem::exists(dir_name + "\\hacks.json"))
        {
            std::ifstream file3(dir_name + "\\hacks.json");
            if (!file3.is_open())
            {
                L_ERROR("Failed to open hacks file!");
                return;
            }

            std::stringstream buffer;
            buffer << file3.rdbuf();
            file3.close();

            std::string json = buffer.str();

            // parse json
            try
            {
                auto j = nlohmann::json::parse(json);
                hacks::load(&j);
            }
            catch (const std::exception &e)
            {
                L_ERROR("Failed to parse hacks file: {}", e.what());
            }
        }

        // save config to recreate missing files
        save(dir_name);
    }

    void save(std::string dir_name)
    {
        // create directory if it doesn't exist
        std::filesystem::create_directory(dir_name);

        // save config to "settings.json"
        std::ofstream file(dir_name + "\\settings.json");
        if (!file.is_open())
        {
            L_ERROR("Failed to open config file!");
            return;
        }

        nlohmann::json j;
        j["reposition_windows"] = reposition_windows;
        j["menu_hotkey"] = menu_hotkey;
        j["menu_animation_length"] = menu_animation_length;
        j["menu_size"] = menu_size;
        j["border_size"] = border_size;
        j["window_rounding"] = window_rounding;
        j["window_snap"] = window_snap;
        j["menu_color"] = {menu_color.r, menu_color.g, menu_color.b, menu_color.a};
        j["frame_color"] = {frame_color.r, frame_color.g, frame_color.b, frame_color.a};
        j["bg_color"] = {bg_color.r, bg_color.g, bg_color.b, bg_color.a};
        j["text_color"] = {text_color.r, text_color.g, text_color.b, text_color.a};
        j["disabled_color"] = {disabled_color.r, disabled_color.g, disabled_color.b, disabled_color.a};
        j["text_color_rainbow"] = text_color_rainbow;
        j["menu_color_rainbow"] = menu_color_rainbow;
        j["menu_rainbow_speed"] = menu_rainbow_speed;
        j["menu_rainbow_brightness"] = menu_rainbow_brightness;

        file << j.dump(4);
        file.close();

        // save window positions to "windows.json"
        std::ofstream file2(dir_name + "\\windows.json");
        if (!file2.is_open())
        {
            L_ERROR("Failed to open window positions file!");
            return;
        }

        file2 << globals::window_positions.dump(4);
        file2.close();

        // save hacks settings to "hacks.json"
        std::ofstream file3(dir_name + "\\hacks.json");
        if (!file3.is_open())
        {
            L_ERROR("Failed to open hacks file!");
            return;
        }

        nlohmann::json j2;
        hacks::save(&j2);
        file3 << j2.dump(4);
        file3.close();
    }
}