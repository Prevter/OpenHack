#include "classic.hpp"

namespace openhack::gui {

    void ClassicTheme::setStyles() {
        auto &style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(4, 4);
        style.WindowRounding = config::get<float>("menu.windowRounding");
        style.FramePadding = ImVec2(4, 2);
        style.FrameRounding = config::get<float>("menu.frameRounding");
        style.PopupRounding = config::get<float>("menu.frameRounding");
        style.ItemSpacing = ImVec2(12, 2);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 15.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 5.0f;
        style.GrabRounding = 3.0f;
        style.WindowBorderSize = config::get<float>("menu.borderSize");
        style.WindowMinSize = ImVec2(32, 32);
        style.DisplayWindowPadding = ImVec2(0, 0);
        style.ScaleAllSizes(config::get<float>("menu.uiScale"));

        auto &colors = style.Colors;
        colors[ImGuiCol_Text] = config::get<Color>("menu.color.text");
        colors[ImGuiCol_TextDisabled] = config::get<Color>("menu.color.textDisabled");

        colors[ImGuiCol_WindowBg] = config::get<Color>("menu.color.background");
        colors[ImGuiCol_FrameBg] = config::get<Color>("menu.color.secondary");
        colors[ImGuiCol_TitleBg] =
        colors[ImGuiCol_TitleBgActive] =
        colors[ImGuiCol_TitleBgCollapsed] =
                config::get<Color>("menu.color.accent");

        colors[ImGuiCol_Button] = config::get<Color>("menu.color.primary");
        colors[ImGuiCol_ButtonHovered] = config::get<Color>("menu.color.hovered");
        colors[ImGuiCol_ButtonActive] = config::get<Color>("menu.color.clicked");

        colors[ImGuiCol_FrameBgHovered] = config::get<Color>("menu.color.secondary");

        colors[ImGuiCol_ScrollbarBg] = config::get<Color>("menu.color.secondary");
        colors[ImGuiCol_ScrollbarGrab] = config::get<Color>("menu.color.primary");
        colors[ImGuiCol_ScrollbarGrabHovered] = config::get<Color>("menu.color.hovered");
        colors[ImGuiCol_ScrollbarGrabActive] = config::get<Color>("menu.color.clicked");

        colors[ImGuiCol_CheckMark] = config::get<Color>("menu.color.primary");
        colors[ImGuiCol_SliderGrab] = config::get<Color>("menu.color.primary");
        colors[ImGuiCol_SliderGrabActive] = config::get<Color>("menu.color.clicked");

        colors[ImGuiCol_Border] = config::get<Color>("menu.color.border");
        colors[ImGuiCol_BorderShadow] = config::get<Color>("menu.color.border");

        colors[ImGuiCol_PopupBg] = config::get<Color>("menu.color.background");
        colors[ImGuiCol_Header] = config::get<Color>("menu.color.primary");
        colors[ImGuiCol_HeaderHovered] = config::get<Color>("menu.color.hovered");
        colors[ImGuiCol_HeaderActive] = config::get<Color>("menu.color.clicked");
    }

    void ClassicTheme::loadPalette() {
        config::set("menu.color.text", Color(1.0f, 1.0f, 1.0f, 1.0f)); // Text color
        config::set("menu.color.textDisabled", Color(0.5f, 0.5f, 0.5f, 0.95f)); // Disabled text color
        config::set("menu.color.background", Color(0.14f, 0.14f, 0.14f, 0.9f)); // Window background
        config::set("menu.color.accent", Color(0.1f, 0.6f, 0.53f, 1.0f)); // Title background
        config::set("menu.color.primary", Color(0.15f, 0.48f, 0.62f, 1.00f)); // Buttons, etc.
        config::set("menu.color.secondary", Color(0.09f, 0.09f, 0.09f, 0.59f)); // Frame background
        config::set("menu.color.border", Color(0.0f, 0.0f, 0.0f, 1.0f)); // Border color
        config::set("menu.color.hovered", Color(0.12f, 0.43f, 0.65f, 1.00f)); // Item hovered
        config::set("menu.color.clicked", Color(0.11f, 0.36f, 0.55f, 1.00f)); // Item clicked
    }

}