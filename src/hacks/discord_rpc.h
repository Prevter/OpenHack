#pragma once
#include "../pch.h"
#include "hacks.h"
#include <discord_rpc.h>

#include "../bindings/GJGameLevel.h"

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

        void initialize_discord();
        void deinitialize_discord();

        static DiscordRPC *get_instance();
        static void change_state(State state, robtop::GJGameLevel *level = nullptr);

    private:
        static DiscordRPC *instance;
        bool m_enabled, m_initialized = false;
        uint64_t m_start_time;
        uint64_t m_last_update_time;
        float m_update_interval = 5.0f;
        DiscordEventHandlers m_handlers;

        robtop::GJGameLevel *m_level = nullptr;
        State m_state = MENU;

        // Configurable options
        char m_menu_detail[60] = "Browsing Menus";
        char m_menu_state[60] = "";
        char m_game_detail[60] = "{name} by {author}";
        char m_game_state[60] = "PB {best}%";
        char m_edit_detail[60] = "Editing {name}";
        char m_edit_state[60] = "{objects} objects";

        char m_current_detail[60] = "";
        char m_current_state[60] = "";
        char m_current_large_image[60] = "cool";
        char m_current_large_text[60] = "";
        char m_current_small_image[60] = "";
        char m_current_small_text[60] = "";
    };
}