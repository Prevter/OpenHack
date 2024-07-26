#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    struct DiscordRPCState {
        std::string state;
        std::string details;
        std::string imageKey;
        std::string imageText;
        PlayLayer *playLayer;
        LevelEditorLayer *editorLayer;
    };

    /// @brief Discord Rich Presence integration
    class DiscordRPC : public EmbeddedHack {
    public:
        DiscordRPC() : EmbeddedHack("Discord RPC", "discord_rpc") {}

        void onInit() override;
        void onDraw() override;
        void update() override;
        bool isCheating() override { return false; }

    private:
        /// @brief Gets all strings for the current game state
        /// @return The current game state
        static DiscordRPCState getState();

        /// @brief Update the presence
        static void updatePresence();

        /// @brief Gets the string from config and replaces all tokens with their values
        /// @param id The string to replace tokens in
        /// @return The string with tokens replaced
        static std::string replaceTokens(const char *id, PlayLayer *playLayer = nullptr, LevelEditorLayer *editorLayer = nullptr);
    };

}