#include "pch.h"
#include "injector/injector.h"

#include "config.h"
#include "updater.h"

#include "hacks/hacks.h"
#include "hook.h"
#include "menu/menu.h"
#include "hooks/MenuLayer.h"
#include "bindings/bindings.h"

DWORD WINAPI MainThread(LPVOID param)
{
#if _DEBUG
    logger::init(true, true, "log.txt");
    utils::set_console_title("OpenHack - Geometry Dash");
#else
    logger::init(false, true, "log.txt");
#endif

    L_INFO("Loading OpenHack " PROJECT_VERSION "...");
    L_INFO("Game version: {}", utils::get_game_version());

    // Check if it's december (for snow particles)
    auto t = std::time(nullptr);
    struct tm tm;
    localtime_s(&tm, &t);
    globals::is_december = tm.tm_mon == 11;

    // Initialize hooks / load hacks
    hook::init();
    config::load();
    injector::load_dlls();

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
        injector::load();
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        DisableThreadLibraryCalls(hModule);
        break;
    default:
        break;
    }
    return TRUE;
}