#include "shortcuts.h"
#include "../menu/gui.h"
#include "../menu/keybinds.h"

namespace hacks
{
    PlayLayer *Shortcuts::play_layer = nullptr;

    Shortcuts::Shortcuts() {}
    void Shortcuts::init() {}
    void Shortcuts::late_init() {}

    void create_button(const char *id, const char *text, std::function<void()> callback, float width = -1.f)
    {
        if (gui::ImButton(text, width))
            callback();
        keybinds::add_menu_keybind(id, text, callback);
    }

    void open_options()
    {
        auto layer = OptionsLayer::create();
        cocos2d::CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
        layer->showLayer(false);
    }

    void restart_level()
    {
        auto play_layer = Shortcuts::get_play_layer();
        if (play_layer)
            play_layer->resetLevel();
    }

    void toggle_practice()
    {
        auto play_layer = Shortcuts::get_play_layer();
        if (play_layer)
        {
            // bool is_practice = play_layer->is_practice();
            // play_layer->togglePracticeMode(!is_practice);
        }
    }

    void open_resources()
    {
        auto path = utils::get_current_directory() + "/Resources";
        utils::open_folder(path.c_str());
    }

    void open_appdata()
    {
        auto path = utils::get_save_directory();
        utils::open_folder(path.c_str());
    }

    void Shortcuts::draw(bool embedded)
    {
        gui::Begin("Shortcuts");

        // Options
        create_button("shortcuts.options", "Show Options", open_options);

        // Gameplay
        create_button("shortcuts.restart", "Restart Level", restart_level);
        create_button("shortcuts.practice", "Practice Mode", toggle_practice);

        // Open folders
        create_button("shortcuts.resources", "Resources", open_resources);
        create_button("shortcuts.appdata", "AppData", open_appdata);

        gui::End();
    }

    void Shortcuts::update() {}
    void Shortcuts::load(nlohmann::json *data) {}
    void Shortcuts::save(nlohmann::json *data) {}

    bool Shortcuts::load_keybind(keybinds::Keybind *keybind)
    {
        if (keybind->id == "shortcuts.options")
        {
            keybind->callback = open_options;
            return true;
        }
        else if (keybind->id == "shortcuts.restart")
        {
            keybind->callback = restart_level;
            return true;
        }
        else if (keybind->id == "shortcuts.practice")
        {
            keybind->callback = toggle_practice;
            return true;
        }
        else if (keybind->id == "shortcuts.resources")
        {
            keybind->callback = open_resources;
            return true;
        }
        else if (keybind->id == "shortcuts.appdata")
        {
            keybind->callback = open_appdata;
            return true;
        }
        return false;
    }

    void Shortcuts::playLayer_init(PlayLayer *self, GJGameLevel *level)
    {
        play_layer = self;
    }

    void Shortcuts::playLayer_destructor(PlayLayer *self)
    {
        play_layer = nullptr;
    }
}