#include "discord_rpc.h"
#include "../menu/gui.h"

namespace hacks
{
    DiscordRPC *DiscordRPC::instance = nullptr;

    DiscordRPC::DiscordRPC()
    {
        m_enabled = true;
        m_start_time = time(NULL);
        m_level = nullptr;
    }

    void DiscordRPC::init()
    {
        instance = this;

        // set default values
        strcpy_s(m_current_large_text, (std::string("Geometry Dash ") + utils::get_game_version()).c_str());
    }
    void DiscordRPC::late_init() {}

    void DiscordRPC::initialize_discord()
    {
        memset(&m_handlers, 0, sizeof(m_handlers));
        Discord_Initialize("1055528380956672081", &m_handlers, 1, NULL);
        m_initialized = true;
    }

    void DiscordRPC::deinitialize_discord()
    {
        if (!m_initialized)
            return;

        Discord_Shutdown();
        m_initialized = false;
    }

    void DiscordRPC::draw(bool embedded)
    {
        if (!embedded)
            return;

        gui::ImToggleButton("Discord RPC", &m_enabled, [this]()
                            {
            gui::ImInputText("Menu Detail", m_menu_detail, 60);
            gui::ImInputText("Menu State", m_menu_state, 60);
            ImGui::Separator();
            gui::ImInputText("Playing Detail", m_game_detail, 60);
            gui::ImInputText("Playing State", m_game_state, 60);
            ImGui::Separator();
            gui::ImInputText("Editor Detail", m_edit_detail, 60);
            gui::ImInputText("Editor State", m_edit_state, 60);
            ImGui::Separator();
            ImGui::InputFloat("Update Interval", &m_update_interval, 0.1f, 1.0f, "%.1f s");
            if (m_update_interval < 1.0f)
                m_update_interval = 1.0f; });

        // keybinds::add_menu_keybind("discord_rpc", "Discord RPC", [this]()
        //                            { m_enabled = !m_enabled; });
    }

    void DiscordRPC::update()
    {
        if (!m_enabled)
        {
            // if disabled, make sure to deinitialize discord
            deinitialize_discord();
            return;
        }

        // if enabled, make sure to always keep discord initialized
        if (!m_initialized)
            initialize_discord();

        // wait for update interval
        if (time(NULL) - m_last_update_time < m_update_interval)
            return;

        m_last_update_time = time(NULL);

        DiscordRichPresence discord_presence;
        memset(&discord_presence, 0, sizeof(discord_presence));
        discord_presence.startTimestamp = m_start_time;
        discord_presence.largeImageKey = m_current_large_image;
        discord_presence.largeImageText = m_current_large_text;
        discord_presence.smallImageKey = m_current_small_image;
        discord_presence.smallImageText = m_current_small_text;
        discord_presence.state = m_current_state;
        discord_presence.details = m_current_detail;
        Discord_UpdatePresence(&discord_presence);
    }

    DiscordRPC *DiscordRPC::get_instance()
    {
        return instance;
    }

    void replace_lambda(std::string &str, std::string value, std::function<std::string()> func)
    {
        size_t pos = str.find(value);
        if (pos != std::string::npos)
            str.replace(pos, value.length(), func());
    }

    inline bool is_robtop_level(GJGameLevel *level)
    {
        int id = level->m_levelID.value();
        return (id > 0 && id < 100) || (id >= 3001 && id <= 6000);
    }

    inline const char *get_difficulty_asset(GJGameLevel *level)
    {
        if (level->m_autoLevel)
            return "auto";

        if (level->m_ratingsSum != 0)
            level->m_difficulty = static_cast<GJDifficulty>(level->m_ratingsSum / 10);

        if (level->m_demon)
        {
            switch (level->m_demonDifficulty)
            {
            case 3:
                return "easy_demon";
            case 4:
                return "medium_demon";
            case 5:
                return "insane_demon";
            case 6:
                return "extreme_demon";
            default:
            case 0:
                return "hard_demon";
            }
        }

        switch (level->m_difficulty)
        {
        case GJDifficulty::Easy:
            return "easy";
        case GJDifficulty::Normal:
            return "normal";
        case GJDifficulty::Hard:
            return "hard";
        case GJDifficulty::Harder:
            return "harder";
        case GJDifficulty::Insane:
            return "insane";
        case GJDifficulty::Demon:
            return "hard_demon";
        default:
            return "na";
        }

        return "na";
    }

    inline bool is_platformer(GJGameLevel *level)
    {
        return level->m_levelLength == 5;
    }

    void DiscordRPC::change_state(State state, GJGameLevel *level)
    {
        if (!instance)
            return;

        instance->m_state = state;

        if (level)
            instance->m_level = level;

        if (state == GAME)
        {
            if (!instance->m_level)
            {
                // just in case the level is null
                strcpy_s(instance->m_current_detail, "Playing a level");
                strcpy_s(instance->m_current_state, "");
                strcpy_s(instance->m_current_small_image, "");
                strcpy_s(instance->m_current_small_text, "");
                return;
            }

            std::string state = instance->m_game_state;
            std::string detail = instance->m_game_detail;

            replace_lambda(state, "{name}", []()
                           { return instance->m_level->m_levelName; });
            replace_lambda(state, "{author}", []()
                           { return std::string(is_robtop_level(instance->m_level) ? "RobTop" : instance->m_level->m_creatorName); });
            replace_lambda(state, "{stars}", []()
                           { return std::to_string(instance->m_level->m_stars.value()); });
            replace_lambda(state, "{id}", []()
                           { return std::to_string(instance->m_level->m_levelID.value()); });
            replace_lambda(state, "{best}", []()
                           { return std::to_string(instance->m_level->m_normalPercent.value()); });

            replace_lambda(detail, "{name}", []()
                           { return instance->m_level->m_levelName; });
            replace_lambda(detail, "{author}", []()
                           { return std::string(is_robtop_level(instance->m_level) ? "RobTop" : instance->m_level->m_creatorName); });
            replace_lambda(detail, "{stars}", []()
                           { return std::to_string(instance->m_level->m_stars.value()); });
            replace_lambda(detail, "{id}", []()
                           { return std::to_string(instance->m_level->m_levelID.value()); });
            replace_lambda(detail, "{best}", []()
                           { return std::to_string(instance->m_level->m_normalPercent.value()); });

            strcpy_s(instance->m_current_detail, detail.c_str());
            strcpy_s(instance->m_current_state, state.c_str());
            strcpy_s(instance->m_current_small_image, get_difficulty_asset(instance->m_level));
            strcpy_s(
                instance->m_current_small_text,
                fmt::format("{} {}",
                            instance->m_level->m_stars.value(),
                            is_platformer(instance->m_level) ? "🌙" : "⭐")
                    .c_str());
        }
        else if (state == EDIT)
        {
            if (!instance->m_level)
            {
                // just in case the level is null
                strcpy_s(instance->m_current_detail, "Editing a level");
                strcpy_s(instance->m_current_state, "");
                strcpy_s(instance->m_current_small_image, "");
                strcpy_s(instance->m_current_small_text, "");
                return;
            }

            std::string state = instance->m_edit_state;
            std::string detail = instance->m_edit_detail;

            replace_lambda(state, "{name}", []()
                           { return instance->m_level->m_levelName; });
            replace_lambda(state, "{objects}", []()
                           { return std::to_string(instance->m_level->m_objectCount.value()); });

            replace_lambda(detail, "{name}", []()
                           { return instance->m_level->m_levelName; });
            replace_lambda(detail, "{objects}", []()
                           { return std::to_string(instance->m_level->m_objectCount.value()); });

            strcpy_s(instance->m_current_detail, detail.c_str());
            strcpy_s(instance->m_current_state, state.c_str());
            strcpy_s(instance->m_current_small_image, "editor");
            strcpy_s(instance->m_current_small_text, "");
        }
        else
        {
            instance->m_level = nullptr;
            strcpy_s(instance->m_current_detail, instance->m_menu_detail);
            strcpy_s(instance->m_current_state, instance->m_menu_state);
            strcpy_s(instance->m_current_small_image, "");
            strcpy_s(instance->m_current_small_text, "");
        }
    }

    void DiscordRPC::load(nlohmann::json *data)
    {
        m_enabled = data->value("discord.enabled", true);
        m_update_interval = data->value("discord.update_interval", 5.0f);
        std::string menu_detail = data->value("discord.menu_detail", "Browsing Menus");
        std::string menu_state = data->value("discord.menu_state", "");
        std::string game_detail = data->value("discord.game_detail", "{name} by {author}");
        std::string game_state = data->value("discord.game_state", "PB {best}%");
        std::string edit_detail = data->value("discord.edit_detail", "Editing {name}");
        std::string edit_state = data->value("discord.edit_state", "{objects} objects");

        strcpy_s(m_menu_detail, menu_detail.c_str());
        strcpy_s(m_menu_state, menu_state.c_str());
        strcpy_s(m_game_detail, game_detail.c_str());
        strcpy_s(m_game_state, game_state.c_str());
        strcpy_s(m_edit_detail, edit_detail.c_str());
        strcpy_s(m_edit_state, edit_state.c_str());

        if (m_enabled)
            initialize_discord();
    }

    void DiscordRPC::save(nlohmann::json *data)
    {
        data->emplace("discord.enabled", m_enabled);
        data->emplace("discord.update_interval", m_update_interval);
        data->emplace("discord.menu_detail", m_menu_detail);
        data->emplace("discord.menu_state", m_menu_state);
        data->emplace("discord.game_detail", m_game_detail);
        data->emplace("discord.game_state", m_game_state);
        data->emplace("discord.edit_detail", m_edit_detail);
        data->emplace("discord.edit_state", m_edit_state);
    }

    bool DiscordRPC::load_keybind(keybinds::Keybind *keybind)
    {
        if (keybind->id == "discord_rpc")
        {
            keybind->callback = [this]()
            {
                m_enabled = !m_enabled;
            };
            return true;
        }

        return false;
    }
}