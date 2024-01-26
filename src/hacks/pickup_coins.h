#pragma once
#include "../pch.h"
#include "hacks.h"

namespace hacks
{
    class PickupCoins : public Hack
    {
    public:
        PickupCoins();
        virtual void init() override;
        virtual void late_init() override;
        virtual void draw(bool embedded = false) override;
        virtual void update() override;
        virtual void load(nlohmann::json *data) override;
        virtual void save(nlohmann::json *data) override;
        virtual std::string get_id() override { return "auto_pickup_coins"; }
        virtual bool load_keybind(keybinds::Keybind *keybind) override;

        bool is_enabled() { return m_enabled; }

        // hooks:
        static void playLayer_init(PlayLayer *self, GJGameLevel *level);
        static void playLayer_addObject(PlayLayer *self, GameObject *object);
        static void playLayer_resetLevel(PlayLayer *self);

    private:
        static PickupCoins *instance;
        bool m_enabled = false;
        std::vector<GameObject*> m_coin_objects;
    };
}