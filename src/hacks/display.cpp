#include "display.h"
#include "../menu/gui.h"

#include "../bindings/bindings.h"

#define MIN_FPS 30.f

namespace hacks
{
    DisplayHack::DisplayHack() {}
    void DisplayHack::init() {}
    void DisplayHack::late_init() {}

    void DisplayHack::update_framerate()
    {
        cocos2d::CCApplication::sharedApplication()->toggleVerticalSync(m_vsync);
        if (m_vsync)
            return;

        float frametime;
        if (m_fps_unlock)
        {
            if (m_fps < MIN_FPS)
                m_fps = MIN_FPS;

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
        if (!initialized)
            return;

        gui::Begin("Display");

        gui::PushWidth(WINDOW_WIDTH / 2 - 15);
        if (gui::ImInputFloat("##fps", &m_fps, "%.0f FPS"))
        {
            if (m_fps < MIN_FPS)
                m_fps = MIN_FPS;

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
        if (!initialized && robtop::GameManager_sharedState_h && robtop::GameManager_getGameVariable_h && robtop::GameManager_setGameVariable_h)
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