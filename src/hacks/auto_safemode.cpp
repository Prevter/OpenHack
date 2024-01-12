#include "auto_safemode.h"
#include "../menu/gui.h"

#include "speedhack.h"

namespace hacks
{
    AutoSafeMode::AutoSafeMode()
    {
    }

    bool can_use_autosafemode = false;

    void AutoSafeMode::init() {}
    void AutoSafeMode::late_init()
    {
        for (auto &hack : hacks::all_hacks)
        {
            if (hack->get_id() == "universal.safemode")
            {
                m_safemode = hack;
                break;
            }
        }

        if (m_safemode->has_warnings())
        {
            L_WARN("AutoSafeMode: Safe Mode has warnings, disabling");
        }
        else
        {
            can_use_autosafemode = true;
        }
    }

    void AutoSafeMode::draw(bool embedded)
    {
        if (!embedded || !can_use_autosafemode)
            return;

        std::string text = fmt::format("Auto Safe Mode ({})", m_has_cheats ? "ON" : "OFF");
        if (gui::ImToggleButton(text.c_str(), &m_enabled))
        {
            if (!m_enabled)
            {
                // if we're disabling it, apply "Safe Mode" settings
                m_safemode->apply_patch();
            }
        }
    }

    void AutoSafeMode::update()
    {
        if (!can_use_autosafemode)
            return;

        if (!m_enabled)
            return;

        // check if any cheats are enabled
        m_has_cheats = false;
        for (auto &hack : hacks::all_hacks)
        {
            if (hack->is_cheat() && hack->is_enabled())
            {
                m_has_cheats = true;
                break;
            }
        }

        // if not, also verify that speedhack is not enabled
        auto speedhack = (Speedhack *)hacks::hacks[0];
        if (!m_has_cheats)
            m_has_cheats = speedhack->is_cheating();

        // if we have cheats enabled, enable safe mode
        if (m_has_cheats)
        {
            // grab opcodes from the "Safe Mode" hack
            auto opcodes = m_safemode->get_opcodes();
            for (auto &opcode : opcodes)
            {
                // otherwise, enable it
                hacks::write_opcode(opcode, true);
            }
        }
        else
        {
            // otherwise, just use whatever "Safe Mode" is set to
            m_safemode->apply_patch();
        }
    }

    void AutoSafeMode::load(nlohmann::json *data)
    {
        m_enabled = data->value("universal.autosafemode", false);
    }

    void AutoSafeMode::save(nlohmann::json *data)
    {
        data->emplace("universal.autosafemode", m_enabled);
    }
}