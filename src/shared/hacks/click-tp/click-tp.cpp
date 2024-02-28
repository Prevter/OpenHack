#include "click-tp.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void ClickTeleport::onInit() {
        // Set the default value
        config::setIfEmpty("hack.click_tp.enabled", false);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("click_tp.enabled", []() {
            bool enabled = !config::get<bool>("hack.click_tp.enabled");
            config::set("hack.click_tp.enabled", enabled);
        });
    }

    void ClickTeleport::onDraw() {
        gui::checkbox("Click Teleport", "hack.click_tp.enabled");
        gui::tooltip("Use right click to teleport to the location to the mouse cursor.");
        menu::keybinds::addMenuKeybind("click_tp.enabled", "Click Teleport", []() {
            bool enabled = !config::get<bool>("hack.click_tp.enabled", false);
            config::set("hack.click_tp.enabled", enabled);
        });
    }

    ImVec2 screenToGame(ImVec2 screenPos, gd::PlayLayer *playLayer) {
        auto cameraPos = playLayer->m_gameState().m_cameraPosition();
        auto cameraScale = playLayer->m_gameState().m_cameraScale();
        auto cameraAngle = playLayer->m_gameState().m_cameraAngle();

        // Rotate the position around the camera angle
        // TODO: Fix rotation
        auto angle = utils::degToRad(cameraAngle);
        auto rotatedPos = utils::rotateVector(screenPos, angle);

        // Scale the position
        auto scaledPos = ImVec2(rotatedPos.x / cameraScale, rotatedPos.y / cameraScale);

        // Add the camera position
        auto point = ImVec2(cameraPos.x + scaledPos.x, cameraPos.y + scaledPos.y);

        return point;
    }

    void ClickTeleport::update() {
        if (!config::get<bool>("hack.click_tp.enabled", false)) return;

        // Always show the cursor
        gd::cocos2d::CCEGLView::sharedOpenGLView()->showCursor(true);

        // Check if the right mouse button is pressed
        if (!utils::isKeyPressed("RMB")) return;

        // Check if the player is in the play layer
        if (auto *playLayer = gd::PlayLayer::get()) {
            auto *player = playLayer->m_player1();
            if (!player) return;

            ImVec2 mousePos = ImGui::GetMousePos();
            auto framePos = utils::screenToFrame(mousePos);

            // Get the position to teleport to
            auto teleportPos = screenToGame({framePos.x, framePos.y}, playLayer);

            // Teleport the player
            player->m_position({teleportPos.x, teleportPos.y});
        }
    }

    bool ClickTeleport::isCheating() {
        return config::get<bool>("hack.click_tp.enabled", false);
    }

}