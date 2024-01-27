#include "pickup_coins.h"
#include "../menu/gui.h"
#include "../menu/keybinds.h"

#include "../bindings/PlayLayer.h"

namespace hacks
{
    PickupCoins *PickupCoins::instance = nullptr;

    PickupCoins::PickupCoins()
    {
        instance = this;
    }

    void PickupCoins::init() {}
    void PickupCoins::late_init() {}

    void PickupCoins::draw(bool embedded)
    {
        if (!embedded)
            return;

        gui::ImToggleButton("Auto Pickup Coins", &m_enabled);
        keybinds::add_menu_keybind("auto_pickup_coins", "Pickup Coins", [&]()
                                   { m_enabled = !m_enabled; });
    }

    void PickupCoins::update() {}

    void PickupCoins::load(nlohmann::json *data)
    {
        m_enabled = data->value("auto_pickup_coins.enabled", false);
    }

    void PickupCoins::save(nlohmann::json *data)
    {
        data->emplace("auto_pickup_coins.enabled", m_enabled);
    }

    bool PickupCoins::load_keybind(keybinds::Keybind *keybind)
    {
        if (keybind->id == "auto_pickup_coins")
        {
            keybind->callback = [&]()
            {
                m_enabled = !m_enabled;
            };
            return true;
        }

        return false;
    }

    void PickupCoins::playLayer_init(PlayLayer *self, GJGameLevel *level)
    {
        if (!instance)
            return;

        instance->m_coin_objects.clear();
    }

    void PickupCoins::playLayer_addObject(PlayLayer *self, GameObject *object)
    {
        if (!instance)
            return;

        uint32_t id = ((robtop::GameObject*)object)->get_id();
        if (id == 142 || id == 1329)
        {
            instance->m_coin_objects.push_back(object);
        }
    }

    void PickupCoins::playLayer_resetLevel(PlayLayer *self)
    {
        if (!instance || !instance->m_enabled)
            return;

        for (auto coin : instance->m_coin_objects)
        {
            if (coin == nullptr)
                continue;

            ((robtop::PlayLayer*)self)->pickupCoin((robtop::GameObject*)coin);
            self->destroyObject(coin);
        }
    }
}