#pragma once
#include "pch.h"

namespace hook
{
    void init();

    // Menu manager
    void set_menu_hotkey(std::string hotkey);
    void set_menu_toggle_callback(void (*callback)());
    void set_menu_draw_callback(void (*callback)());
    void set_menu_init_callback(void (*callback)());

    void try_bind_method(std::string name, void *method, void **original, std::string pattern, std::string library = "");

    extern bool lock_inputs;
    extern HWND window_handle;
}