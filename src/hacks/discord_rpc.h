#pragma once
#include "../pch.h"
#include "hacks.h"
#include <discord_rpc.h>

namespace hacks
{
    class DiscordRPC : public Hack
    {
    public:
        enum State
        {
            MENU,
            GAME,
            EDIT
        };

        DiscordRPC();
        virtual void init() override;
        virtual void late_init() override;
        virtual void draw(bool embedded = false) override;
        virtual void update() override;
        virtual void load(nlohmann::json *data) override;
        virtual void save(nlohmann::json *data) override;
        virtual std::string get_id() override { return "discord_rpc"; }
        virtual bool load_keybind(keybinds::Keybind *keybind) override;

        void initialize_discord();
        void deinitialize_discord();

        void update_labels();

        static DiscordRPC *get_instance();
        static void change_state(State state, GJGameLevel *level = nullptr);

    private:
        static DiscordRPC *instance;
        bool m_enabled, m_initialized = false;
        uint64_t m_start_time, m_level_start_time;
        uint64_t m_last_update_time;
        float m_update_interval = 5.0f;
        bool m_level_time = false;
        DiscordEventHandlers m_handlers;

        GJGameLevel *m_level = nullptr;
        State m_state = MENU;

        // Configurable options
        std::string m_menu_detail = "Browsing Menus";
        std::string m_menu_state = "";
        std::string m_game_detail = "{name} by {author}";
        std::string m_game_state = "PB {best}%";
        std::string m_edit_detail = "Editing {name}";
        std::string m_edit_state = "{objects} objects";

        std::string m_current_detail = "";
        std::string m_current_state = "";
        std::string m_current_large_image = "cool";
        std::string m_current_large_text = "";
        std::string m_current_small_image = "";
        std::string m_current_small_text = "";
    };
}