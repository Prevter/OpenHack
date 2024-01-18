#pragma once
#include <string>

namespace utils
{
    extern uintptr_t base_addr;
    extern uintptr_t cocos_addr;
    extern bool is_2_200;

    void init();

    void set_console_title(const char *title);

    std::string get_current_directory();

    void reset_key_states();                // called once every frame, resets key states
    bool is_key_pressed(uint32_t keycode);  // checks if a key was pressed during this frame
    std::string key_name(uint32_t keycode); // converts a keycode to a readable name
    uint32_t decode_key(std::string key);   // converts a key name to a keycode

    const char *get_game_version();

    bool compare_version(const char *version_filter);

    std::string get_request(const char *url);
    bool download_file(const char *url, const char *path, float *progress = nullptr);

    void extract_zip(const char *zip_path, const char *output_path);
}