#include "discord-rpc.hpp"
#include "../../menu/menu.hpp"
#include "../labels/labels.hpp"

#include <discord_rpc.h>

namespace openhack::hacks {

    static void handleDiscordError(int errcode, const char *message) {
        L_ERROR("Discord: error ({}, {})", errcode, message);
    }

    void DiscordRPC::onInit() {
        // Set defaults
        // General
        config::setIfEmpty("hack.discord_rpc.enabled", false);
        config::setIfEmpty("hack.discord_rpc.update_interval", 1000.0f);
        config::setIfEmpty("hack.discord_rpc.show_time", true);
        config::setIfEmpty("hack.discord_rpc.level_time", false);
        // Buttons
        config::setIfEmpty("hack.discord_rpc.button1", false);
        config::setIfEmpty("hack.discord_rpc.button1_text", "Button 1");
        config::setIfEmpty("hack.discord_rpc.button1_url", "");
        config::setIfEmpty("hack.discord_rpc.button2", false);
        config::setIfEmpty("hack.discord_rpc.button2_text", "Button 2");
        config::setIfEmpty("hack.discord_rpc.button2_url", "");
        // Large icon
        config::setIfEmpty("hack.discord_rpc.large_image_key", "circle");
        config::setIfEmpty("hack.discord_rpc.large_image_text", "Geometry Dash ({username})");
        // Menu
        config::setIfEmpty("hack.discord_rpc.menu.image_key", "");
        config::setIfEmpty("hack.discord_rpc.menu.image_text", "");
        config::setIfEmpty("hack.discord_rpc.menu.details", "Browsing menus");
        config::setIfEmpty("hack.discord_rpc.menu.state", "");
        // Level (normal mode)
        config::setIfEmpty("hack.discord_rpc.level.image_key", "{difficulty}");
        config::setIfEmpty("hack.discord_rpc.level.image_text", "{stars}{star_emoji} (ID: {id})");
        config::setIfEmpty("hack.discord_rpc.level.details", "{name} by {author}");
        config::setIfEmpty("hack.discord_rpc.level.state", "Progress: {progress}% (Best {best}%)");
        // Level (platformer mode)
        config::setIfEmpty("hack.discord_rpc.platformer.image_key", "{difficulty}");
        config::setIfEmpty("hack.discord_rpc.platformer.image_text", "{stars}{star_emoji} (ID: {id})");
        config::setIfEmpty("hack.discord_rpc.platformer.details", "{name} by {author}");
        config::setIfEmpty("hack.discord_rpc.platformer.state", "Best time: {best} s.");
        // Editor
        config::setIfEmpty("hack.discord_rpc.editor.image_key", "editor");
        config::setIfEmpty("hack.discord_rpc.editor.image_text", "Editing a level");
        config::setIfEmpty("hack.discord_rpc.editor.details", "Working on \"{name}\"");
        config::setIfEmpty("hack.discord_rpc.editor.state", "{objects} objects");

        // Initialize Discord RPC
        DiscordEventHandlers handlers;
        memset(&handlers, 0, sizeof(handlers));
        handlers.errored = handleDiscordError;
        Discord_Initialize("1212016614325624852", &handlers, 1, nullptr);

        config::setGlobal("discord_rpc.startTime", std::time(nullptr));
        config::setGlobal("discord_rpc.levelTime", std::time(nullptr));
    }

    void DiscordRPC::onDraw() {
        if (gui::toggleSetting("Discord RPC", "hack.discord_rpc.enabled", [&]() {
            gui::width(200);

            // Update interval
            gui::inputFloat("Update interval", "hack.discord_rpc.update_interval", 500.f, FLT_MAX, "%.1f ms");
            gui::tooltip("How often presence should be updated");

            ImGui::Separator();

            // Timestamps
            gui::checkbox("Show time", "hack.discord_rpc.show_time");
            gui::tooltip("Show total time spent in the game");
            gui::checkbox("Show level time", "hack.discord_rpc.level_time");
            gui::tooltip("Use time spent in the current level instead of total time");

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Menu")) {
                // Image
                gui::inputText("Image key##menu", "hack.discord_rpc.menu.image_key", 128);
                gui::tooltip("ID or URL of the image to show");
                gui::inputText("Image text##menu", "hack.discord_rpc.menu.image_text");
                gui::tooltip("Text to show when hovering over the image");

                // State and details
                gui::inputText("Details##menu", "hack.discord_rpc.menu.details");
                gui::inputText("State##menu", "hack.discord_rpc.menu.state");
            }
            if (ImGui::CollapsingHeader("Normal level")) {
                // Image
                gui::inputText("Image key##level", "hack.discord_rpc.level.image_key", 128);
                gui::tooltip("ID or URL of the image to show");
                gui::inputText("Image text##level", "hack.discord_rpc.level.image_text");
                gui::tooltip("Text to show when hovering over the image");

                // State and details
                gui::inputText("Details##level", "hack.discord_rpc.level.details");
                gui::inputText("State##level", "hack.discord_rpc.level.state");
            }
            if (ImGui::CollapsingHeader("Platformer level")) {
                // Image
                gui::inputText("Image key##platformer", "hack.discord_rpc.platformer.image_key", 128);
                gui::tooltip("ID or URL of the image to show");
                gui::inputText("Image text##platformer", "hack.discord_rpc.platformer.image_text");
                gui::tooltip("Text to show when hovering over the image");

                // State and details
                gui::inputText("Details##platformer", "hack.discord_rpc.platformer.details");
                gui::inputText("State##platformer", "hack.discord_rpc.platformer.state");
            }
            if (ImGui::CollapsingHeader("Editor")) {
                // Image
                gui::inputText("Image key##editor", "hack.discord_rpc.editor.image_key", 128);
                gui::tooltip("ID or URL of the image to show");
                gui::inputText("Image text##editor", "hack.discord_rpc.editor.image_text");
                gui::tooltip("Text to show when hovering over the image");

                // State and details
                gui::inputText("Details##editor", "hack.discord_rpc.editor.details");
                gui::inputText("State##editor", "hack.discord_rpc.editor.state");
            }

            ImGui::Separator();

            // Icons
            gui::inputText("Large image", "hack.discord_rpc.large_image_key", 128);
            gui::tooltip(
                    "ID or URL of the large image\nSupported ids: \"icon\", \"circle\", \"meltdown\", \"subzero\", \"world\"");
            gui::inputText("Large image text", "hack.discord_rpc.large_image_text");
            gui::tooltip("Text to show when hovering over the large image");

            ImGui::Separator();

            // Buttons
            gui::checkbox("Button 1", "hack.discord_rpc.button1");
            gui::tooltip("Show a button with a link to a website");
            if (config::get<bool>("hack.discord_rpc.button1")) {
                gui::inputText("Button 1 text", "hack.discord_rpc.button1_text");
                gui::inputText("Button 1 url", "hack.discord_rpc.button1_url", 128);
            }
            gui::checkbox("Button 2", "hack.discord_rpc.button2");
            gui::tooltip("Show a button with a link to a website");
            if (config::get<bool>("hack.discord_rpc.button2")) {
                gui::inputText("Button 2 text", "hack.discord_rpc.button2_text");
                gui::inputText("Button 2 url", "hack.discord_rpc.button2_url", 128);
            }

            gui::width();
        })) {
            // Clear/Update presence
            if (config::get<bool>("hack.discord_rpc.enabled")) {
                updatePresence();
            } else {
                Discord_ClearPresence();
            }
        }
    }

    void DiscordRPC::update() {
        // Update presence every X seconds
        static time_t lastUpdate = 0.0f;
        if (config::get<bool>("hack.discord_rpc.enabled") &&
            (float) (utils::getTime() - lastUpdate) >= config::get<float>("hack.discord_rpc.update_interval")) {
            updatePresence();
            lastUpdate = utils::getTime();
        }
    }

    std::string DiscordRPC::replaceTokens(const char *id, gd::PlayLayer *playLayer, gd::LevelEditorLayer *editorLayer) {
        auto str = config::get<std::string>(id);
        return Labels::replaceTokens(str, playLayer, editorLayer);
    }

    /// @brief Get the current state of the game and return corresponding strings
    DiscordRPCState DiscordRPC::getState() {
        if (auto playLayer = gd::PlayLayer::get()) {
            if (playLayer->m_level()->m_levelLength() == gd::GJLevelLength::Platformer) {
                // Playing a platformer level
                return {
                        replaceTokens("hack.discord_rpc.platformer.state", playLayer),
                        replaceTokens("hack.discord_rpc.platformer.details", playLayer),
                        replaceTokens("hack.discord_rpc.platformer.image_key", playLayer),
                        replaceTokens("hack.discord_rpc.platformer.image_text", playLayer),
                        playLayer,
                };
            }
            // Playing a level
            return {
                    replaceTokens("hack.discord_rpc.level.state", playLayer),
                    replaceTokens("hack.discord_rpc.level.details", playLayer),
                    replaceTokens("hack.discord_rpc.level.image_key", playLayer),
                    replaceTokens("hack.discord_rpc.level.image_text", playLayer),
                    playLayer,
                    nullptr,
            };
        } else if (auto editorLayer = gd::LevelEditorLayer::get()) {
            // Editing a level
            return {
                    replaceTokens("hack.discord_rpc.editor.state", nullptr, editorLayer),
                    replaceTokens("hack.discord_rpc.editor.details", nullptr, editorLayer),
                    replaceTokens("hack.discord_rpc.editor.image_key", nullptr, editorLayer),
                    replaceTokens("hack.discord_rpc.editor.image_text", nullptr, editorLayer),
                    nullptr,
                    editorLayer,
            };
        } else {
            // Browsing menus
            return {
                    replaceTokens("hack.discord_rpc.menu.state"),
                    replaceTokens("hack.discord_rpc.menu.details"),
                    replaceTokens("hack.discord_rpc.menu.image_key"),
                    replaceTokens("hack.discord_rpc.menu.image_text"),
                    nullptr,
                    nullptr,
            };
        }
    }

    void DiscordRPC::updatePresence() {
        // Update from config
        auto status = getState();
        auto largeImageKey = replaceTokens("hack.discord_rpc.large_image_key", status.playLayer, status.editorLayer);
        auto largeImageText = replaceTokens("hack.discord_rpc.large_image_text", status.playLayer, status.editorLayer);
        bool button0 = config::get<bool>("hack.discord_rpc.button1");
        auto buttonText0 = replaceTokens("hack.discord_rpc.button1_text", status.playLayer, status.editorLayer);
        auto buttonUrl0 = replaceTokens("hack.discord_rpc.button1_url", status.playLayer, status.editorLayer);
        bool button1 = config::get<bool>("hack.discord_rpc.button2");
        auto buttonText1 = replaceTokens("hack.discord_rpc.button2_text", status.playLayer, status.editorLayer);
        auto buttonUrl1 = replaceTokens("hack.discord_rpc.button2_url", status.playLayer, status.editorLayer);

        // Send presence
        DiscordRichPresence presence;
        memset(&presence, 0, sizeof(presence));
        presence.state = status.state.c_str();
        presence.details = status.details.c_str();
        presence.largeImageKey = largeImageKey.c_str();
        presence.largeImageText = largeImageText.c_str();
        presence.smallImageKey = status.imageKey.c_str();
        presence.smallImageText = status.imageText.c_str();

        // Set total time
        if (config::get<bool>("hack.discord_rpc.show_time")) {
            presence.startTimestamp = config::getGlobal<std::time_t>("discord_rpc.startTime", 0);
        }

        if (status.playLayer && config::get<bool>("hack.discord_rpc.level_time")) {
            presence.startTimestamp = config::getGlobal<std::time_t>("discord_rpc.levelTime", 0);
        }

        // Buttons
        presence.buttons[0].active = button0 ? 1 : 0;
        presence.buttons[0].label = buttonText0.c_str();
        presence.buttons[0].url = buttonUrl0.c_str();
        presence.buttons[1].active = button1 ? 1 : 0;
        presence.buttons[1].label = buttonText1.c_str();
        presence.buttons[1].url = buttonUrl1.c_str();

        // Send update
        Discord_UpdatePresence(&presence);
    }

}