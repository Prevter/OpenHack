#pragma once
#include "../pch.h"
#include "hacks.h"

#include "../bindings/PlayLayer.h"

namespace hacks
{
    class StartposSwitcher : public Hack
    {
    public:
        StartposSwitcher();
        virtual void init() override;
        virtual void late_init() override;
        virtual void draw(bool embedded = false) override;
        virtual void update() override;
        virtual void load(nlohmann::json *data) override;
        virtual void save(nlohmann::json *data) override;
        virtual std::string get_id() override { return "startpos_switch"; }
        virtual bool load_keybind(keybinds::Keybind *keybind) override;

        void choose_start_pos(int32_t index);

        // hooks:
        static void playLayer_init(robtop::PlayLayer *self, robtop::GJGameLevel *level);
        static void playLayer_lateInit(robtop::PlayLayer *self);
        static void playLayer_destructor(robtop::PlayLayer *self);
        static void playLayer_addObject(robtop::PlayLayer *self, robtop::GameObject *object);

    private:
        static StartposSwitcher *instance;

        robtop::PlayLayer *m_play_layer = nullptr;
        std::vector<robtop::GameObject *> m_startpos_objects;

        cocos2d::CCLabelBMFont *m_label = nullptr;

        bool m_enabled = false;
        bool m_show_label = true;

        uint32_t m_prev_keybind = 81; // Q
        uint32_t m_next_keybind = 69; // E

        int32_t m_current_index = 0;
    };
}