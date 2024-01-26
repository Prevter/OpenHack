#include "globals.h"

namespace globals
{
    Vec2 screen_size;
    bool reset_windows = false;
    json window_positions;
    float animation = 1.0f;
    bool animation_done;
    int random_direction;
    EaseInAnimation *animation_action;
    bool is_december = false;
    uint32_t blur_shader;
    ImFont *main_font, *title_font;
    ImVec4 current_color;
    char search[64] = {0};
}