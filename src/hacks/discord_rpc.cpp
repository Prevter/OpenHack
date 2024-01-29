#include "discord_rpc.h"
#include "../menu/gui.h"

#include "../bindings/bindings.h"

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
        m_current_large_text = fmt::format("Geometry Dash {}", utils::get_game_version());
        m_current_small_image = "";
        m_current_small_text = "";
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

        gui::ImToggleButton(
            "Discord RPC", &m_enabled,
            [this]()
            {
                char buffer[256];

                strcpy_s(buffer, m_menu_detail.c_str());
                gui::ImInputText("Menu Detail", buffer, 60);
                m_menu_detail = buffer;

                strcpy_s(buffer, m_menu_state.c_str());
                gui::ImInputText("Menu State", buffer, 60);
                m_menu_state = buffer;

                ImGui::Separator();

                strcpy_s(buffer, m_game_detail.c_str());
                gui::ImInputText("Playing Detail", buffer, 60);
                m_game_detail = buffer;

                strcpy_s(buffer, m_game_state.c_str());
                gui::ImInputText("Playing State", buffer, 60);
                m_game_state = buffer;

                ImGui::Separator();

                strcpy_s(buffer, m_edit_detail.c_str());
                gui::ImInputText("Editor Detail", buffer, 60);
                m_edit_detail = buffer;

                strcpy_s(buffer, m_edit_state.c_str());
                gui::ImInputText("Editor State", buffer, 60);
                m_edit_state = buffer;

                ImGui::Separator();
                ImGui::InputFloat("Update Interval", &m_update_interval, 0.1f, 1.0f, "%.1f s");
                if (m_update_interval < 1.0f)
                    m_update_interval = 1.0f;

                gui::ImToggleButton("Level Time", &m_level_time);
            });

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

        update_labels();

        DiscordRichPresence discord_presence;
        memset(&discord_presence, 0, sizeof(discord_presence));
        discord_presence.startTimestamp = (m_state == GAME && m_level_time) ? m_level_start_time : m_start_time;
        discord_presence.largeImageKey = m_current_large_image.c_str();
        discord_presence.largeImageText = m_current_large_text.c_str();
        discord_presence.smallImageKey = m_current_small_image.c_str();
        discord_presence.smallImageText = m_current_small_text.c_str();
        discord_presence.state = m_current_state.c_str();
        discord_presence.details = m_current_detail.c_str();
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

    void replace_all_tags(std::map<std::string, std::function<std::string()>> tags, std::string &str)
    {
        for (auto &tag : tags)
            replace_lambda(str, tag.first, tag.second);
    }

    inline bool is_robtop_level(robtop::GJGameLevel *level)
    {
        int id = level->m_levelID.value();
        return (id > 0 && id < 100) || (id >= 3001 && id <= 6000);
    }

    inline const char *get_difficulty_asset(robtop::GJGameLevel *level)
    {
        if (level->is_auto_level())
            return "auto";

        if (level->m_ratingsSum != 0)
            level->m_difficulty = static_cast<robtop::GJDifficulty>(level->m_ratingsSum / 10);

        if (level->is_demon())
        {
            switch (level->get_demon_difficulty())
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
        case robtop::GJDifficulty::kGJDifficultyEasy:
            return "easy";
        case robtop::GJDifficulty::kGJDifficultyNormal:
            return "normal";
        case robtop::GJDifficulty::kGJDifficultyHard:
            return "hard";
        case robtop::GJDifficulty::kGJDifficultyHarder:
            return "harder";
        case robtop::GJDifficulty::kGJDifficultyInsane:
            return "insane";
        case robtop::GJDifficulty::kGJDifficultyDemon:
            return "hard_demon";
        default:
            return "na";
        }

        return "na";
    }

    inline bool is_platformer(robtop::GJGameLevel *level)
    {
        return level->get_length() == 5;
    }

    void DiscordRPC::update_labels()
    {
        const std::map<std::string, std::function<std::string()>> tags = {
            {"{name}", []()
             { return instance->m_level->m_levelName; }},
            {"{stars}", []()
             { return std::to_string(instance->m_level->get_stars()); }},
            {"{id}", []()
             { return std::to_string(instance->m_level->m_levelID.value()); }},
            {"{best}", []()
             { return std::to_string(instance->m_level->get_normal_percent()); }},
            {"{objects}", []()
             { return std::to_string(instance->m_level->m_objectCount.value()); }},
            {"{author}", []()
             { return is_robtop_level(instance->m_level) ? std::string("RobTop") : instance->m_level->m_creatorName; }},
        };

        std::string detail, state;
        switch (m_state)
        {
        case MENU:
            detail = m_menu_detail;
            state = m_menu_state;
            m_current_small_image = "";
            m_current_small_text = "";
            break;
        case GAME:
            detail = m_game_detail;
            state = m_game_state;
            break;
        case EDIT:
            detail = m_edit_detail;
            state = m_edit_state;
            instance->m_current_small_image = "editor";
            instance->m_current_small_text = "";
            break;
        }

        if (!m_level)
        {
            // just in case the level is null
            m_current_detail = detail;
            m_current_state = state;
            return;
        }

        replace_all_tags(tags, state);
        replace_all_tags(tags, detail);

        m_current_detail = detail;
        m_current_state = state;

        if (m_state == GAME)
        {
            instance->m_current_small_image = get_difficulty_asset(instance->m_level);
            instance->m_current_small_text = fmt::format("{} {}", instance->m_level->get_stars(), is_platformer(instance->m_level) ? "🌙" : "⭐");
        }
    }

    void DiscordRPC::change_state(State state, robtop::GJGameLevel *level)
    {
        if (!instance)
            return;

        instance->m_state = state;

        if (state == MENU)
            instance->m_level = nullptr;

        if (level)
            instance->m_level = level;

        if (state == GAME && level) // only called from PlayLayer::init
            instance->m_level_start_time = time(NULL);
    }

    void DiscordRPC::load(nlohmann::json *data)
    {
        m_enabled = data->value("discord.enabled", true);
        m_update_interval = data->value("discord.update_interval", 5.0f);
        m_menu_detail = data->value("discord.menu_detail", "Browsing Menus");
        m_menu_state = data->value("discord.menu_state", "");
        m_game_detail = data->value("discord.game_detail", "{name} by {author}");
        m_game_state = data->value("discord.game_state", "PB {best}%");
        m_edit_detail = data->value("discord.edit_detail", "Editing {name}");
        m_edit_state = data->value("discord.edit_state", "{objects} objects");

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