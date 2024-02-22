#include "auto-pickup-coins.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    std::vector<gd::GameObject*> AutoPickupCoins::m_coins;

    void AutoPickupCoins::onInit() {
        // Set the default value
        config::setIfEmpty("hack.auto_pickup_coins.enabled", false);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("auto_pickup_coins.enabled", []() {
            bool enabled = !config::get<bool>("hack.auto_pickup_coins.enabled");
            config::set("hack.auto_pickup_coins.enabled", enabled);
        });
    }

    void AutoPickupCoins::onDraw() {
        gui::checkbox("Auto Pickup Coins", "hack.auto_pickup_coins.enabled");
        gui::tooltip("Picks up all coins in the level automatically.");
        menu::keybinds::addMenuKeybind("auto_pickup_coins.enabled", "Instant Complete", []() {
            bool enabled = !config::get<bool>("hack.auto_pickup_coins.enabled", false);
            config::set("hack.auto_pickup_coins.enabled", enabled);
        });
    }

    bool AutoPickupCoins::isCheating() {
        return config::get<bool>("hack.auto_pickup_coins.enabled", false);
    }

    void AutoPickupCoins::initLevel() {
        m_coins.clear();
    }

    void AutoPickupCoins::addObject(gd::GameObject *object) {
        uint32_t id = object->m_objectID();
        if (id == 142 || id == 1329) {
            m_coins.push_back(object);
        }
    }

    void AutoPickupCoins::resetLevel() {
        if (!config::get<bool>("hack.auto_pickup_coins.enabled", false)) return;

        // Get PlayLayer
        auto *playLayer = gd::PlayLayer::get();

        // Pickup all coins from m_coins
        for (auto *coin : m_coins) {
            if (coin == nullptr) continue;

            // Pickup the coin
            playLayer->pickupCoin(coin);
            playLayer->destroyObject(coin);
        }
    }

}