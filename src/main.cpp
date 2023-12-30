#include "pch.h"
#include "winmm.h"

#include "config.h"

#include "hacks/hacks.h"
#include "hook.h"
#include "menu/menu.h"
#include "hooks/MenuLayer.h"

bool initialized = false;

DWORD WINAPI MainThread(LPVOID param)
{
#if _DEBUG
    logger::init(true, true, "log.txt");
#else
    logger::init(false, true, "log.txt");
#endif

    L_INFO("Loading OpenHack...");

    utils::set_console_title("OpenHack - Geometry Dash");

    hooks::MenuLayer::on_init = []()
    {
        if (initialized)
            return;

        hook::set_menu_hotkey(config::menu_hotkey);
        hook::set_menu_toggle_callback(menu::toggle);
        hook::set_menu_draw_callback(menu::draw);
        hook::set_menu_init_callback(menu::init);

        initialized = true;
        L_INFO("OpenHack loaded successfully!");
    };

    hook::init();
    config::load();

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        DisableThreadLibraryCalls(hModule);
        break;
    default:
        break;
    }
    return TRUE;
}