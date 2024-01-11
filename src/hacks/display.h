#pragma once
#include "../pch.h"
#include "hacks.h"

namespace hacks
{
    class DisplayHack : public Hack
    {
        inline static bool initialized = false;

    public:
        DisplayHack();
        virtual void init() override;
        virtual void draw(bool embedded = false) override;
        virtual void update() override;
        virtual void load(nlohmann::json *data) override;
        virtual void save(nlohmann::json *data) override;
        virtual std::string get_id() override { return "display"; }

        void update_framerate();

    private:
        float m_fps = 240;
        bool m_fps_unlock = false;
        bool m_fullscreen = false;
        bool m_vsync = false;
        bool m_show_fps = false;
    };
}