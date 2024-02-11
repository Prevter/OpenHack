#include "openhack.hpp"
#include <imgui.h>

namespace openhack
{
    void initialize()
    {
        L_INFO("Initializing OpenHack v" OPENHACK_VERSION "...");
        L_INFO("Game version: {}", utils::getGameVersion());

        try
        {
            gd::init();
            L_INFO("Initialized gd.hpp");
        }
        catch (const std::exception &e)
        {
            L_ERROR("Failed to initialize gd.hpp: {}", e.what());
        }
    }

    void deinitialize()
    {
    }
}