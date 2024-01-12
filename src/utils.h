#pragma once
#include <string>

namespace utils
{
    void set_console_title(const char *title);

    std::string get_current_directory();

    uint32_t decode_key(std::string key);

    const char *get_game_version();

    bool compare_version(const char *version_filter);

    std::string get_request(const char *url);
    bool download_file(const char *url, const char *path, float *progress = nullptr);

    void extract_zip(const char *zip_path, const char *output_path);
}