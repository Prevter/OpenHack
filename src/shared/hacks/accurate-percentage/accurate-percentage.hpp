#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief
    class AccuratePercentage : public EmbeddedHack {
    public:
        AccuratePercentage() : EmbeddedHack("Accurate Percentage", "accurate_percentage") {}

        void onInit() override;
        void onDraw() override;
        void update() override {}
        bool isCheating() override { return false; }

        /// @brief Get the current progress percentage of the level.
        /// @param oldEvaluation Whether to use the old evaluation method. (2.1)
        /// @return The current progress percentage of the level.
        static float getPercentage(bool oldEvaluation = false);

    public:
        /// @brief PlayLayer::postUpdate hook
        static void postUpdate();
    };

}