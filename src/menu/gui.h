#pragma once
#include "../pch.h"

#define WINDOW_WIDTH 240

namespace gui
{
    void Begin(const char *name, bool *open = (bool *)false);
    void BeginPrompt(const char *name, bool *open = (bool *)false);
    void End();

    // wrappers for ImGui functions
    void ImText(const char *fmt, ...);
    bool ImCheckbox(const char *label, bool *v);
    bool ImButton(const char *label, float width = -1.f);
    bool ImInputFloat(const char *label, float *v, const char *format = "%.3f");
    bool ImInputInt(const char *label, int *v, int step);
    bool ImInputInt2(const char *label, int *v);
    bool ImInputText(const char *label, char *buf, size_t buf_size);
    bool ImCombo(const char *label, int *current_item, const char *const items, int items_count);
    bool ImColorEdit3(const char *label, float *col, ImGuiColorEditFlags flags = 0);
    bool ImColorEdit4(const char *label, float *col, ImGuiColorEditFlags flags = 0);
    void ImProgressBar(float fraction, float width = -1.f);

    // a toggle button to use instead of regular checkbox, which supports settings
    bool ImToggleButton(const char *label, bool *v, std::function<void()> settings = nullptr, float width = -1.f);

    // a label with a settings button
    void ImPopupSetting(const char *label, std::function<void()> settings, float width = -1.f);

    // a button to set a keybind
    bool ImKeybind(const char *label, uint32_t *key, float width = -1.f, size_t max_chars = 15, bool show_delete = true);

    void PushWidth(float width);
    void PopWidth();
}