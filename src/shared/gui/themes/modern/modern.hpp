#pragma once

#include "../themes.hpp"

namespace openhack::gui {
    class ModernTheme : public Theme {
    public:
        std::string getName() override { return "Modern"; }

        void setStyles() override;

        void loadPalette() override;

        bool inputText(const char *label, std::string *value, int bufferSize, const char *placeholder, ImGuiTextFlags flags) override;

        bool inputFloat(const char *label, float *value, float min, float max, const char *format) override;

        bool button(const char *label, const ImVec2 &size) override;

        bool checkbox(const char *label, bool *value) override;

        void popupSettings(const char* label, const std::function<void()> &content, ImVec2 size) override;

        bool toggleSetting(const char *label, bool *value, const std::function<void()> &popupDraw, ImVec2 size, float minWidth) override;
    };
}