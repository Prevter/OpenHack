#include "pch.h"

#include "config.h"

#include "hacks/hacks.h"
#include "hook.h"
#include "menu/menu.h"
#include "menu/keybinds.h"
#include "bindings/bindings.h"

$execute
{
    L_INFO("Loading OpenHack " PROJECT_VERSION "...");
    L_INFO("Game version: {}", utils::get_game_version());

    // Initialize hooks / load hacks
    L_TRACE("Looking for bindings...");
    robtop::init_bindings();
    L_TRACE("Initializing hooks...");
    hook::init();
    L_TRACE("Loading hacks...");
    hacks::init();
    L_TRACE("Loading config...");
    config::load();

    // Initialize keybinds
    L_TRACE("Initializing keybinds...");
    keybinds::init();

    // Check if it's december (for snow particles)
    auto t = std::time(nullptr);
    struct tm tm;
    localtime_s(&tm, &t);
    globals::is_december = tm.tm_mon == 11;

    L_TRACE("Setting up menu...");
    hook::set_menu_toggle_callback(menu::toggle);
    hook::set_menu_init_callback(menu::init);
    hook::set_menu_draw_callback(menu::draw);

    L_INFO("OpenHack loaded successfully!");
}