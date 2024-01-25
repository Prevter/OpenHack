#pragma once
#include "../pch.h"
#include "hacks.h"

namespace hacks
{
    class Shortcuts : public Hack
    {
    public:
        Shortcuts();
        virtual void init() override;
        virtual void late_init() override;
        virtual void draw(bool embedded = false) override;
        virtual void update() override;
        virtual void load(nlohmann::json *data) override;
        virtual void save(nlohmann::json *data) override;
        virtual std::string get_id() override { return "shortcuts"; }
        virtual bool load_keybind(keybinds::Keybind *keybind) override;
    };
}