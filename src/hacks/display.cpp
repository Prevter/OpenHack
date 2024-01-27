#include "display.h"
#include "../menu/gui.h"
#include "../menu/keybinds.h"

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

            auto gameManager = GameManager::sharedState();
            gameManager->m_customFPSTarget = m_fps;
            gameManager->updateCustomFPS();
            update_framerate();
        }
        gui::PopWidth();
        ImGui::SameLine();

        keybinds::shortcut_toggle(
            "display.fps_unlock", "FPS Unlock", &m_fps_unlock,
            [this]()
            {
                GameManager::sharedState()->setGameVariable("0116", m_fps_unlock);
                update_framerate();
            },
            WINDOW_WIDTH / 2);

        keybinds::shortcut_toggle(
            "display.vsync", "Vertical Sync", &m_vsync,
            [this]()
            {
                GameManager::sharedState()->setGameVariable("0030", m_vsync);
                update_framerate();
            });

        // if (gui::ImToggleButton("Show FPS", &m_show_fps))
        // {
        //     GameManager::sharedState()->setGameVariable("0115", m_show_fps);
        //     cocos2d::CCDirector::sharedDirector()->toggleShowFPS(m_show_fps, "", {0.f, 0.f});
        // }

        // if (gui::ImToggleButton("Fullscreen", &m_fullscreen))
        // {
        //     GameManager::sharedState()->setGameVariable("0029", m_fullscreen);
        //     cocos2d::CCApplication::sharedApplication()->setFullscreen(m_fullscreen);
        // }

        gui::End();
    }

    void DisplayHack::update()
    {
        if (!initialized)
        {
            initialized = true;

            // get initial values
            auto gameManager = GameManager::sharedState();
            m_fps = gameManager->m_customFPSTarget;
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

    bool DisplayHack::load_keybind(keybinds::Keybind *keybind)
    {
        if (keybind->id == "display.fps_unlock")
        {
            keybind->callback = [this]()
            {
                GameManager::sharedState()->setGameVariable("0116", m_fps_unlock);
                update_framerate();
            };
            return true;
        }
        else if (keybind->id == "display.vsync")
        {
            keybind->callback = [this]()
            {
                GameManager::sharedState()->setGameVariable("0030", m_vsync);
                update_framerate();
            };
            return true;
        }

        return false;
    }
}