#pragma once
#include <string>

namespace utils
{
    void set_console_title(const char *title);

    std::string get_current_directory();

    uint32_t decode_key(std::string key);
}