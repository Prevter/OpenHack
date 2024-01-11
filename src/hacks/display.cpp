#include "display.h"
#include "../menu/gui.h"

namespace robtop
{
    const char *getGameManager_pat = "558BEC6AFF68????64A1000000005051A1????33C5508D45F464A300000000A1????85C075406840050000";
    const char *getGameVariable_pat = "558BEC83E4F86AFF68????64A1000000005083EC38A1????33C489442430535657A1????33C4508D44244864A3000000008BF98B75088D4C24286A03";
    const char *setGameVariable_pat = "558BEC83E4F86AFF68????64A1000000005083EC58A1????33C489442450535657A1????33C4508D44246864A3000000008BD9895C2410";

    class GameManager;
    robtop::GameManager *(__stdcall *sharedGameManager_h)();
    bool(__thiscall *getGameVariable_h)(robtop::GameManager *manager, const char *key);
    void(__thiscall *setGameVariable_h)(robtop::GameManager *manager, const char *key, bool value);

#define FPS_OFFSET 900

    class GameManager
    {
    public:
        inline static GameManager *sharedState()
        {
            return sharedGameManager_h();
        }
        inline float getFps()
        {
            return *(float *)((uintptr_t)this + FPS_OFFSET);
        }
        inline void setFps(float fps)
        {
            *(float *)((uintptr_t)this + FPS_OFFSET) = fps;
        }
        inline bool getGameVariable(const char *key)
        {
            if (!getGameVariable_h)
                return false;
            return getGameVariable_h(this, key);
        }
        inline void setGameVariable(const char *key, bool value)
        {
            if (!setGameVariable_h)
                return;
            setGameVariable_h(this, key, value);
        }
    };

    const char *toggleShowFPS_name = "?toggleShowFPS@CCDirector@cocos2d@@QAEX_NV?$basic_string@DU?$char";
    // void(__thiscall *CCDirector_toggleShowFPS)(cocos2d::CCDirector *self, bool show, cocos2d::CCString str, cocos2d::CCPoint pos);

}

namespace hacks
{
    DisplayHack::DisplayHack()
    {
    }

    void DisplayHack::init()
    {
        // setup hooks
        robtop::getGameVariable_h = (bool(__thiscall *)(robtop::GameManager *, const char *))
            patterns::find_pattern(robtop::getGameVariable_pat);
        robtop::sharedGameManager_h = (robtop::GameManager * (__stdcall *)())
            patterns::find_pattern(robtop::getGameManager_pat);
        robtop::setGameVariable_h = (void(__thiscall *)(robtop::GameManager *, const char *, bool))
            patterns::find_pattern(robtop::setGameVariable_pat);

        // robtop::CCDirector_toggleShowFPS = (void(__thiscall *)(cocos2d::CCDirector *, bool, cocos2d::CCString, cocos2d::CCPoint))
        //     GetProcAddress(GetModuleHandle("libcocos2d.dll"), robtop::toggleShowFPS_name);

        // check if hooks are valid
        if (!robtop::sharedGameManager_h)
        {
            L_ERROR("Failed to find GameManager::sharedState");
            return;
        }
        if (!robtop::getGameVariable_h)
        {
            L_ERROR("Failed to find GameManager::getGameVariable");
            return;
        }
        if (!robtop::setGameVariable_h)
        {
            L_ERROR("Failed to find GameManager::setGameVariable");
            return;
        }
        // if (!robtop::CCDirector_toggleShowFPS)
        // {
        //     L_ERROR("Failed to find CCDirector::toggleShowFPS");
        //     return;
        // }
    }

    void DisplayHack::update_framerate()
    {
        cocos2d::CCApplication::sharedApplication()->toggleVerticalSync(m_vsync);
        if (m_vsync)
            return;

        float frametime;
        if (m_fps_unlock)
        {
            if (m_fps < 60)
                m_fps = 60;

            frametime = 1.0f / m_fps;
        }
        else
        {
            frametime = 1.0f / 60.0f;
        }

        cocos2d::CCDirector::sharedDirector()->setAnimationInterval(frametime);
    }

    void DisplayHack::draw(bool embedded)
    {
        gui::Begin("Display");

        gui::PushWidth(WINDOW_WIDTH / 2 - 15);
        if (gui::ImInputFloat("##fps", &m_fps, "%.0f FPS"))
        {
            if (m_fps < 60)
                m_fps = 60;

            robtop::GameManager::sharedState()->setFps((float)m_fps);
            update_framerate();
        }
        gui::PopWidth();
        ImGui::SameLine();

        if (gui::ImToggleButton("Unlock FPS", &m_fps_unlock, nullptr, WINDOW_WIDTH / 2))
        {
            robtop::GameManager::sharedState()->setGameVariable("0116", m_fps_unlock);
            update_framerate();
        }
        if (gui::ImToggleButton("Vertical Sync", &m_vsync))
        {
            robtop::GameManager::sharedState()->setGameVariable("0030", m_vsync);
            update_framerate();
        }

        // if (gui::ImToggleButton("Show FPS", &m_show_fps))
        // {
        //     robtop::GameManager::sharedState()->setGameVariable("0115", m_show_fps);
        //     auto director = cocos2d::CCDirector::sharedDirector();
        //     robtop::CCDirector_toggleShowFPS(director, m_show_fps, nullptr, cocos2d::CCPoint(0, 0));
        // }

        // if (gui::ImToggleButton("Fullscreen", &m_fullscreen))
        // {
        //     robtop::GameManager::sharedState()->setGameVariable("0029", m_fullscreen);
        // }

        gui::End();
    }

    void DisplayHack::update()
    {
        if (!initialized)
        {
            initialized = true;

            // get initial values
            auto gameManager = robtop::GameManager::sharedState();
            m_fps = gameManager->getFps();
            m_fullscreen = gameManager->getGameVariable("0029");
            m_vsync = gameManager->getGameVariable("0030");
            m_show_fps = gameManager->getGameVariable("0115");
            m_fps_unlock = gameManager->getGameVariable("0116");
        }
    }

    void DisplayHack::load(nlohmann::json *data)
    {
    }

    void DisplayHack::save(nlohmann::json *data)
    {
    }
}