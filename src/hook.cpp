#include "hook.h"
#include "menu/menu.h"

#include "hacks/hacks.h"
#include "hooks/hooks.h"

namespace hook
{
    bool gl_initialized = false;

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

    // Hooked functions
    void(__thiscall *CCEGLView_swapBuffers)(cocos2d::CCEGLView *);
    void __fastcall swapBuffers_hook(cocos2d::CCEGLView *self)
    {
        if (!menu_draw_callback || !menu_init_callback)
        {
            // Call original function
            CCEGLView_swapBuffers(self);
            return;
        }

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
            auto hwnd = WindowFromDC(*reinterpret_cast<HDC *>(reinterpret_cast<uintptr_t>(window) + 0x244));
            ImGui_ImplWin32_Init(hwnd);
            ImGui_ImplOpenGL3_Init();

            // Initialize menu
            menu_init_callback();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        menu_draw_callback();

        ImGui::EndFrame();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glFlush();

        // Call original function
        CCEGLView_swapBuffers(self);
    }

    void(__thiscall *CCEGLView_pollEvents)(cocos2d::CCEGLView *);
    void __fastcall pollEvents_hook(cocos2d::CCEGLView *self)
    {
        if (!gl_initialized)
        {
            // Call original function
            CCEGLView_pollEvents(self);
            return;
        }

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
            else if (msg.message == WM_KEYDOWN && msg.wParam == menu_hotkey)
            {
                menu_toggle_callback();
            }

            if (!block_input)
                DispatchMessage(&msg);

            ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
        }

        // Call original function
        CCEGLView_pollEvents(self);
    }

    void(__thiscall *CCEGLView_toggleFullScreen)(cocos2d::CCEGLView *, bool);
    void __fastcall toggleFullScreen_hook(cocos2d::CCEGLView *self, bool fullscreen)
    {
        menu::menu_open = false;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        gl_initialized = false;

        // Call original function
        CCEGLView_toggleFullScreen(self, fullscreen);
    }

    void(__thiscall *AppDelegate_applicationWillEnterForeground)(void *);
    void __fastcall AppDelegate_applicationWillEnterForeground_hook(void *self)
    {
        AppDelegate_applicationWillEnterForeground(self);
        ImGui::GetIO().ClearInputKeys();
    }

    // Helper method for creating hooks with debug logging
    void try_bind_method(std::string name, void *method, void **original, std::string pattern, std::string library)
    {
        uintptr_t address = patterns::find_pattern(pattern, library);
        if (address)
        {
            MH_CreateHook((void *)address, method, original);
        }
        else
        {
            L_ERROR("Failed to find " + name + " pattern");
        }
    }

    // Hooking
    void init()
    {
        auto cocos2d_base = GetModuleHandleA("libcocos2d.dll");
        uintptr_t game_base = (uintptr_t)GetModuleHandleA(0);

        MH_Initialize();

        MH_CreateHook(
            GetProcAddress(cocos2d_base, "?swapBuffers@CCEGLView@cocos2d@@UAEXXZ"),
            swapBuffers_hook,
            (void **)&CCEGLView_swapBuffers);
        MH_CreateHook(
            GetProcAddress(cocos2d_base, "?pollEvents@CCEGLView@cocos2d@@QAEXXZ"),
            pollEvents_hook,
            (void **)&CCEGLView_pollEvents);
        MH_CreateHook(
            GetProcAddress(cocos2d_base, "?toggleFullScreen@CCEGLView@cocos2d@@QAEX_N@Z"),
            toggleFullScreen_hook,
            (void **)&CCEGLView_toggleFullScreen);

        try_bind_method(
            "ApplicationWillEnterForeground",
            AppDelegate_applicationWillEnterForeground_hook,
            (void **)&AppDelegate_applicationWillEnterForeground,
            "538B1D????5657FFD38BC88B10");

        hooks::init_all();

        // Initialize hacks here, to make sure the hooks are created
        hacks::init();

        MH_EnableHook(MH_ALL_HOOKS);
    }
}