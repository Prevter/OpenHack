#include "hook.h"
#include "menu/menu.h"
#include "hacks/hacks.h"

#define WRAP_HOOK(hook)                                 \
    if (!(hook).isOk())                                 \
    {                                                   \
        L_ERROR("Failed to hook: {}", (hook).err()); \
    }

namespace hook
{
    bool gl_initialized = false;
    bool lock_inputs = false;
    HWND window_handle;

    // Menu manager
    uint32_t menu_hotkey;
    void (*menu_toggle_callback)();
    void (*menu_draw_callback)();
    void (*menu_init_callback)();

    void set_menu_hotkey(std::string hotkey)
    {
        menu_hotkey = utils::decode_key(hotkey);
    }

    void set_menu_toggle_callback(void (*callback)())
    {
        menu_toggle_callback = callback;
    }

    void set_menu_draw_callback(void (*callback)())
    {
        menu_draw_callback = callback;
    }

    void set_menu_init_callback(void (*callback)())
    {
        menu_init_callback = callback;
    }

    void on_draw(cocos2d::CCEGLView *self)
    {
        if (!menu_draw_callback || !menu_init_callback)
            return;

        auto window = self->getWindow();

        // Initialize openGL if not initialized
        if (!gl_initialized)
        {
            gl_initialized = true;
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            // Initialize ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            window_handle = WindowFromDC(*reinterpret_cast<HDC *>(reinterpret_cast<uintptr_t>(window) + 0x244));
            ImGui_ImplWin32_Init(window_handle);
            ImGui_ImplOpenGL3_Init();

            // Initialize menu
            menu_init_callback();
        }

        lock_inputs = false;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        menu_draw_callback();

        ImGui::EndFrame();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glFlush();
    }

    void on_update(cocos2d::CCEGLView *self)
    {
        if (!gl_initialized)
            return;

        utils::reset_key_states();
        auto &io = ImGui::GetIO();
        bool block_input = false;
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);

            // Block input if ImGui wants to capture it
            if (io.WantCaptureMouse)
            {
                switch (msg.message)
                {
                case WM_LBUTTONDBLCLK:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_MBUTTONDBLCLK:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                case WM_MOUSEACTIVATE:
                case WM_MOUSEHOVER:
                case WM_MOUSEHWHEEL:
                case WM_MOUSELEAVE:
                case WM_MOUSEMOVE:
                case WM_MOUSEWHEEL:
                case WM_NCLBUTTONDBLCLK:
                case WM_NCLBUTTONDOWN:
                case WM_NCLBUTTONUP:
                case WM_NCMBUTTONDBLCLK:
                case WM_NCMBUTTONDOWN:
                case WM_NCMBUTTONUP:
                case WM_NCMOUSEHOVER:
                case WM_NCMOUSELEAVE:
                case WM_NCMOUSEMOVE:
                case WM_NCRBUTTONDBLCLK:
                case WM_NCRBUTTONDOWN:
                case WM_NCRBUTTONUP:
                case WM_NCXBUTTONDBLCLK:
                case WM_NCXBUTTONDOWN:
                case WM_NCXBUTTONUP:
                case WM_RBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_XBUTTONDBLCLK:
                case WM_XBUTTONDOWN:
                case WM_XBUTTONUP:
                    block_input = true;
                }
            }

            if (io.WantCaptureKeyboard)
            {
                switch (msg.message)
                {
                case WM_HOTKEY:
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_KILLFOCUS:
                case WM_SETFOCUS:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                    block_input = true;
                }
            }

            if (!block_input && !lock_inputs)
                DispatchMessage(&msg);

            ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
        }

        // Toggle menu
        if (utils::is_key_pressed(menu_hotkey))
        {
            menu_toggle_callback();
        }
    }

    void uninitialize()
    {
        menu::menu_open = false;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        gl_initialized = false;
    }

    void pollEvents_hook(cocos2d::CCEGLView *self)
    {
        on_update(self);
        self->pollEvents();
    }

    void toggleFullScreen_hook(cocos2d::CCEGLView *self, void *, bool fullscreen, bool borderless)
    {
        uninitialize();
        self->toggleFullScreen(fullscreen, borderless);
    }

    // Hooking
    void init()
    {
        auto cocos_handle = GetModuleHandleA("libcocos2d.dll");
        auto toggleFullScreenSign = "?toggleFullScreen@CCEGLView@cocos2d@@QAEX_N0@Z";
        auto pollEventsSign = "?pollEvents@CCEGLView@cocos2d@@QAEXXZ";

        // Hook CCEGLView::toggleFullScreenSign
        WRAP_HOOK(geode::Mod::get()->hook(
            reinterpret_cast<void *>(GetProcAddress(cocos_handle, toggleFullScreenSign)),
            &toggleFullScreen_hook,
            "cocos2d::CCEGLView::toggleFullScreen",
            tulip::hook::TulipConvention::Thiscall))

        // Hook CCEGLView::pollEvents
        WRAP_HOOK(geode::Mod::get()->hook(
            reinterpret_cast<void *>(GetProcAddress(cocos_handle, pollEventsSign)),
            &pollEvents_hook,
            "cocos2d::CCEGLView::pollEvents",
            tulip::hook::TulipConvention::Thiscall))
    }
}