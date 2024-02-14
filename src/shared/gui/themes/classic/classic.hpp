#pragma once

#include "../themes.hpp"

namespace openhack::gui {
    class ClassicTheme : public Theme {
    public:
        std::string getName() override { return "Classic"; }

        void setStyles() override;

        void loadPalette() override;
    };
}