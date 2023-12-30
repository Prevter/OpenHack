#include "speedhack.h"
#include "../menu/gui.h"

namespace hacks
{
    float speed = 1.0f;
    bool enabled, audio;

    void(__thiscall *CCScheduler_update)(cocos2d::CCScheduler *self, float dt);
    void __fastcall CCScheduler_update_hook(cocos2d::CCScheduler *self, float dt)
    {
        if (enabled)
            dt *= speed;

        CCScheduler_update(self, dt);
    }

    Speedhack::Speedhack()
    {
        enabled = false;
        audio = false;
    }

    void Speedhack::init()
    {
        if (CCScheduler_update)
            return;

        auto cocos2d = GetModuleHandle("libcocos2d.dll");
        auto scheduler = GetProcAddress(cocos2d, "?update@CCScheduler@cocos2d@@UAEXM@Z");
        MH_CreateHook(scheduler, CCScheduler_update_hook, (void **)&CCScheduler_update);
    }

    void Speedhack::draw()
    {
        gui::Begin("Speedhack");

        gui::PushWidth(WINDOW_WIDTH / 2 - 15);
        gui::ImInputFloat("##speed", &speed, "%.2fx");
        gui::PopWidth();
        ImGui::SameLine();
        gui::ImToggleButton("Enabled", &enabled, nullptr, WINDOW_WIDTH / 2);

        // TODO: Implement audio speedhack
        // gui::ImToggleButton("Speedhack Audio", &audio);

        gui::End();
    }

    void Speedhack::update() {}

    void Speedhack::load(nlohmann::json *data)
    {
        speed = data->value("speedhack.speed", 1.0f);
        enabled = data->value("speedhack.enabled", false);
        audio = data->value("speedhack.audio", false);
    }

    void Speedhack::save(nlohmann::json *data)
    {
        data->emplace("speedhack.speed", speed);
        data->emplace("speedhack.enabled", enabled);
        data->emplace("speedhack.audio", audio);
    }
}