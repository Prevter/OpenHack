#include "pch.h"
#include "injector/injector.h"

#include "config.h"
#include "updater.h"

#include "hacks/hacks.h"
#include "hook.h"
#include "menu/menu.h"
#include "hooks/MenuLayer.h"
#include "bindings/bindings.h"

// this gets called immediately after the dll is injected
// because we need to patch the game before it starts
void preload()
{
    injector::load();

    logger::init(false, true, "log.txt");
    utils::init();

    L_INFO("Loading OpenHack " PROJECT_VERSION "...");
    L_INFO("Game version: {}", utils::get_game_version());

    // Initialize hooks / load hacks
    robtop::init_bindings();
    hook::init();
    config::load();
    injector::load_dlls();
}

DWORD WINAPI MainThread(LPVOID param)
{
    // Check if it's december (for snow particles)
    auto t = std::time(nullptr);
    struct tm tm;
    localtime_s(&tm, &t);
    globals::is_december = tm.tm_mon == 11;

    // Check for updates
    if (config::check_updates)
    {
        updater::check_update("prevter/gdopenhack",
                              [](updater::version_t version)
                              {
                                  globals::latest_version = version;
                                  globals::show_update_popup = PROJECT_VERSION != version.version;
                              });
    }

    hook::set_menu_hotkey(config::menu_hotkey);
    hook::set_menu_toggle_callback(menu::toggle);
    hook::set_menu_init_callback(menu::init);
    hook::set_menu_draw_callback(menu::draw);

    L_INFO("OpenHack loaded successfully!");

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        preload();
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        DisableThreadLibraryCalls(hModule);
        break;
    default:
        break;
    }
    return TRUE;
}