#include "speedhack.h"
#include "../menu/gui.h"

namespace hacks
{
    Speedhack *speedhack_instance = nullptr;

    void(__thiscall *CCScheduler_update)(cocos2d::CCScheduler *self, float dt);
    void __fastcall CCScheduler_update_hook(cocos2d::CCScheduler *self, float dt)
    {
        if (speedhack_instance && speedhack_instance->m_enabled)
            dt *= speedhack_instance->m_speed;

        CCScheduler_update(self, dt);
    }

    Speedhack::Speedhack()
    {
        speedhack_instance = this;
    }

    void Speedhack::init()
    {
        if (CCScheduler_update)
            return;

        auto cocos2d = GetModuleHandle("libcocos2d.dll");
        auto scheduler = GetProcAddress(cocos2d, "?update@CCScheduler@cocos2d@@UAEXM@Z");
        MH_CreateHook(scheduler, CCScheduler_update_hook, (void **)&CCScheduler_update);
    }

    void Speedhack::draw(bool embedded)
    {
        gui::Begin("Speedhack");

        gui::PushWidth(WINDOW_WIDTH / 2 - 15);
        gui::ImInputFloat("##speed", &m_speed, "%.2fx");
        gui::PopWidth();
        ImGui::SameLine();
        gui::ImToggleButton("Enabled", &m_enabled, nullptr, WINDOW_WIDTH / 2);

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
}