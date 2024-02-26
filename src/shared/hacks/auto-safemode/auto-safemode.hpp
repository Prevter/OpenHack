#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Automatically enables "Safe Mode" if you have cheats enabled
    class AutoSafemode : public EmbeddedHack {
    public:
        AutoSafemode() : EmbeddedHack("Auto Safe Mode", "autosafemode") {}

        void onInit() override;
        void onDraw() override;
        void update() override;
        bool isCheating() override { return false; }

    public:
        /// @brief Original "Safe Mode" hack component to use opcodes from
        static ToggleComponent *safeModeComponent;
    };

}