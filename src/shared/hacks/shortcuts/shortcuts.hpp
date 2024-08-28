#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Window with shortcuts to various game options
    class Shortcuts : public EmbeddedHack {
    public:
        Shortcuts() : EmbeddedHack("Shortcuts", "shortcuts") {}

        void onInit() override;
        void onDraw() override {}
        void update() override {}
        bool isCheating() override { return false; }

    private:
        // static void patchGame();
        static void uncompleteLevel();
        static void openOptions();
        static void restartLevel();
        static void togglePractice();
        static void openResources();
        static void openAppData();
    };

}