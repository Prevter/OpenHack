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

    void on_draw(cocos2d::CCEGLView *self);
    void on_update(cocos2d::CCEGLView *self);
    void uninitialize();

    extern bool lock_inputs;
    extern HWND window_handle;
}