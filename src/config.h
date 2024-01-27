#pragma once
#include "pch.h"

#define MAIN_DIR "openhack"

namespace config
{
    // config variables
    extern bool always_reposition;  // whether to run reset_windows() every frame

    extern std::string menu_hotkey;                       // key for opening/closing the menu
    extern float menu_animation_length;                   // in seconds, open/close animation length
    extern float menu_size, border_size, window_rounding; // menu customization
    extern int window_snap;                               // snap distance

    extern Color menu_color, bg_color, frame_color, text_color, disabled_color; // menu colors
    extern bool text_color_rainbow;                                             // whether to apply rainbow effect to text color
    extern bool menu_color_rainbow;                                             // whether to use rainbow effect
    extern float menu_rainbow_speed, menu_rainbow_brightness;                   // rainbow settings

    // save/load functions
    void load(std::string dir_name = MAIN_DIR);
    void save(std::string dir_name = MAIN_DIR);
}