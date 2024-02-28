#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Allows you to teleport to the location you click on
    class ClickTeleport : public EmbeddedHack {
    public:
        ClickTeleport() : EmbeddedHack("Click Teleport", "click_tp") {}

        void onInit() override;
        void onDraw() override;
        void update() override;
        bool isCheating() override;

    };

}