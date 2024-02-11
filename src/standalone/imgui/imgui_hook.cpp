#include "imgui_hook.hpp"
#include "../pch.hpp"

#include <GLFW/glfw3.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ImGuiHook
{
    bool m_initialized = false;
    bool isInitialized() { return m_initialized; }

    std::function<void()> m_initCallback = nullptr;
    void setInitCallback(std::function<void()> callback) { m_initCallback = callback; }

    std::function<void()> m_drawCallback = nullptr;
    void setDrawCallback(std::function<void()> callback) { m_drawCallback = callback; }

    HWND windowHandle = nullptr;

    void setup(gd::cocos2d::CCEGLView *view)
    {
        if (m_initialized)
            return;

        auto *window = gd::cocos2d::CCEGLView::getWindow(view);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        windowHandle = WindowFromDC(*reinterpret_cast<HDC *>(reinterpret_cast<uintptr_t>(window) + 0x244));
        ImGui_ImplWin32_Init(windowHandle);
        ImGui_ImplOpenGL3_Init();

        // Clear fonts before calling the init callback
        if (io.Fonts)
            io.Fonts->Clear();

        // Call the init callback
        if (m_initCallback)
            m_initCallback();

        // Setup fonts
        io.Fonts->Build();
        ImGui_ImplOpenGL3_CreateFontsTexture();

        m_initialized = true;
    }

    void handleEvents()
    {
        if (!m_initialized)
            return;

        auto &io = ImGui::GetIO();
        bool blockInput = false;
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
                    blockInput = true;
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
                    blockInput = true;
                }
            }

            if (!blockInput)
                DispatchMessage(&msg);

            ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
        }
    }

    void draw(gd::cocos2d::CCEGLView *view)
    {
        if (!m_initialized)
            setup(view);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (m_drawCallback)
            m_drawCallback();

        ImGui::EndFrame();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glFlush();
    }

    void destroy()
    {
        if (!m_initialized)
            return;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        m_initialized = false;
    }

    void clearInput()
    {
        auto &io = ImGui::GetIO();
        io.ClearInputKeys();
    }
}