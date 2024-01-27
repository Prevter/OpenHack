#include "speedhack.h"
#include "../menu/gui.h"
#include "../menu/keybinds.h"

#include <Geode/modify/CCScheduler.hpp>

namespace hacks
{
    Speedhack *speedhack_instance = nullptr;

    struct CCSchedulerHook : geode::Modify<CCSchedulerHook, cocos2d::CCScheduler>
    {
        void update(float dt)
        {
            if (speedhack_instance && speedhack_instance->is_enabled())
                dt *= speedhack_instance->get_speed();

            return cocos2d::CCScheduler::update(dt);
        }
    };

    Speedhack::Speedhack()
    {
        speedhack_instance = this;
    }

    void Speedhack::init() {}
    void Speedhack::late_init() {}

    void Speedhack::draw(bool embedded)
    {
        gui::Begin("Speedhack");

        gui::PushWidth(WINDOW_WIDTH / 2 - 15);
        gui::ImInputFloat("##speed", &m_speed, "%.2fx");
        gui::PopWidth();
        ImGui::SameLine();
        keybinds::shortcut_toggle("speedhack.enabled", "Speedhack", "Enabled", &m_enabled, nullptr, WINDOW_WIDTH / 2);

        // TODO: Implement audio speedhack
        // gui::ImToggleButton("Speedhack Audio", &audio);

        gui::End();
    }

    void Speedhack::update() {}

    void Speedhack::load(nlohmann::json *data)
    {
        m_speed = data->value("speedhack.speed", 1.0f);
        m_enabled = data->value("speedhack.enabled", false);
        m_audio = data->value("speedhack.audio", false);
    }

    void Speedhack::save(nlohmann::json *data)
    {
        data->emplace("speedhack.speed", m_speed);
        data->emplace("speedhack.enabled", m_enabled);
        data->emplace("speedhack.audio", m_audio);
    }

    bool Speedhack::load_keybind(keybinds::Keybind *keybind)
    {
        if (keybind->id == "speedhack.enabled")
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