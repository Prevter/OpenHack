#pragma once

#include "../themes.hpp"

namespace openhack::gui {
    class GruvboxTheme : public Theme {
    public:
        std::string getName() override { return "Gruvbox"; }

        void setStyles() override;

        void loadPalette() override;

        bool checkbox(const char *label, bool *value) override;

        void popupSettings(const char* label, const std::function<void()> &content, ImVec2 size) override;

        bool toggleSetting(const char *label, bool *value, const std::function<void()> &popupDraw, ImVec2 size, float minWidth) override;
    };
}