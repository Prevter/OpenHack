#pragma once

#include "../themes.hpp"

namespace openhack::gui {
    class MegaHackTheme : public Theme {
    public:
        std::string getName() override { return "MegaHack"; }

        void setStyles() override;

        void loadPalette() override;

        bool button(const char *label, const ImVec2 &size) override;

        bool checkbox(const char *label, bool *value) override;
    };
}