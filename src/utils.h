#pragma once
#include <string>
#include <stdint.h>
#include <map>

namespace utils
{
    void set_console_title(const char *title);

    std::string get_current_directory();
    std::string get_save_directory();
    void open_folder(const char *path);

    void reset_key_states();                // called once every frame, resets key states
    bool is_key_pressed(uint32_t keycode);  // checks if a key was pressed during this frame
    std::string key_name(uint32_t keycode); // converts a keycode to a readable name
    uint32_t decode_key(std::string key);   // converts a key name to a keycode

    const char *get_game_version();

    bool compare_version(const char *version_filter);
}