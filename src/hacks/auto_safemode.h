#pragma once
#include "../pch.h"
#include "hacks.h"

namespace hacks
{
    class AutoSafeMode : public Hack
    {
    public:
        AutoSafeMode();
        virtual void init() override;
        virtual void late_init() override;
        virtual void draw(bool embedded = false) override;
        virtual void update() override;
        virtual void load(nlohmann::json *data) override;
        virtual void save(nlohmann::json *data) override;
        virtual std::string get_id() override { return "autosafemode"; }
    private:
        bool m_enabled = false;
        bool m_has_cheats = false;
        ToggleComponent *m_safemode;
    };
}