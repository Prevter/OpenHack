#pragma once

#include "../themes.hpp"

namespace openhack::gui {
    class MegaHackTheme : public Theme {
    public:
        std::string getName() override { return "MegaHack"; }

        void setStyles() override;

        void loadPalette() override;
    };
}