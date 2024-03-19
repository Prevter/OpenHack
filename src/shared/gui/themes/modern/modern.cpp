#include "modern.hpp"

namespace openhack::gui {

    void ModernTheme::setStyles() {
        auto &style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(5, 4);
        style.WindowRounding = config::get<float>("menu.windowRounding");
        style.FramePadding = ImVec2(4, 3);
        style.FrameRounding = config::get<float>("menu.frameRounding");
        style.PopupRounding = config::get<float>("menu.frameRounding");
        style.ItemSpacing = ImVec2(12, 3);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 15.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 5.0f;
        style.GrabRounding = 3.0f;
        style.WindowBorderSize = config::get<float>("menu.borderSize");
        style.WindowMinSize = ImVec2(32, 32);
        style.DisplayWindowPadding = ImVec2(0, 0);
        style.ScaleAllSizes(config::getGlobal<float>("UIScale"));

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

    void ModernTheme::loadPalette() {
        config::set("menu.color.text", Color(1.0f, 1.0f, 1.0f, 1.0f)); // Text color
        config::set("menu.color.textDisabled", Color(0.5f, 0.5f, 0.5f, 0.95f)); // Disabled text color
        config::set("menu.color.background", Color(0.07f, 0.07f, 0.07f, 0.9f)); // Window background
        config::set("menu.color.accent", Color(0.12f, 0.12f, 0.12f, 1.0f)); // Title background
        config::set("menu.color.primary", Color(0.73f, 0.53f, 0.99f, 1.00f)); // Buttons, etc.
        config::set("menu.color.secondary", Color(0.12f, 0.12f, 0.12f, 0.95f)); // Frame background
        config::set("menu.color.border", Color(0.0f, 0.0f, 0.0f, 1.0f)); // Border color
        config::set("menu.color.hovered", Color(0.58f, 0.42f, 0.79f, 1.00f)); // Item hovered
        config::set("menu.color.clicked", Color(0.44f, 0.31f, 0.58f, 1.00f)); // Item clicked
    }

    bool ModernTheme::inputFloat(const char *label, float *value, float min, float max, const char *format) {
        // Transparent background with a line border on the bottom
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));

        // Add a line border on the bottom
        auto drawList = ImGui::GetWindowDrawList();
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto availWidth = ImGui::CalcItemWidth();

        auto color = config::get<Color>("menu.color.primary");
        color.a = 0.4f;
        drawList->AddLine(ImVec2(cursorPos.x, cursorPos.y + 20), ImVec2(cursorPos.x + availWidth, cursorPos.y + 20), color);
        bool changed = ImGui::InputFloat(label, value, 0.0f, 0.0f, format);

        ImGui::PopStyleColor(3);

        if (*value < min)
            *value = min;
        if (*value > max)
            *value = max;
        return changed;
    }

    bool ModernTheme::button(const char *label, const ImVec2 &size) {
        auto textColor = config::get<Color>("menu.color.text");
        auto buttonColor = config::get<Color>("menu.color.primary");
        auto hoveredColor = config::get<Color>("menu.color.hovered");
        auto clickedColor = config::get<Color>("menu.color.clicked");
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) textColor);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(buttonColor.r, buttonColor.g, buttonColor.b, 0.4f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(hoveredColor.r, hoveredColor.g, hoveredColor.b, 0.4f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(clickedColor.r, clickedColor.g, clickedColor.b, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_Border, (ImVec4) buttonColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);

        if (size.x == 0)
            ImGui::PushItemWidth(-1);

        auto availWidth = ImGui::GetContentRegionAvail().x;
        if (size.x > 0)
            availWidth *= size.x;
        bool clicked = ImGui::Button(label, ImVec2(availWidth, size.y));

        if (size.x == 0)
            ImGui::PopItemWidth();

        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar();

        return clicked;
    }

    bool ModernTheme::checkbox(const char *label, bool *value) {
        ImGui::PushItemWidth(-1.0f);
        auto textColor = *value ? config::get<Color>("menu.color.primary") : config::get<Color>(
                "menu.color.textDisabled");

        if (utils::isSearching()) {
            if (utils::isSearched(label)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.1f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.2f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) textColor);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.1f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.2f));
        }

        bool clicked = ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x, 0));

        ImGui::PopStyleColor(4);

        if (clicked) *value = !*value;
        ImGui::PopItemWidth();
        return clicked;
    }

    bool ModernTheme::toggleSetting(const char *label, bool *value, const std::function<void()> &popupDraw, ImVec2 size,
                                    float minWidth) {
        ImGui::PushItemWidth(-1.0f);
        auto textColor = *value ? config::get<Color>("menu.color.primary") : config::get<Color>(
                "menu.color.textDisabled");

        if (utils::isSearching()) {
            if (utils::isSearched(label)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.1f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.2f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) textColor);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.1f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.2f));
        }

        auto availWidth = ImGui::GetContentRegionAvail().x;
        auto buttonSize = ImVec2(availWidth * 0.88f, 0);
        auto arrowSize = ImVec2(availWidth * 0.12f, 0);

        if (size.x > 0) {
            buttonSize.x *= size.x;
            arrowSize.x *= size.x;
        }

        bool clicked = ImGui::Button(label, buttonSize);
        ImGui::PopStyleColor(3);
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) config::get<Color>("menu.color.text"));
        gui::callback();
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 0);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.1f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.2f));
        ImGui::SetNextItemWidth(arrowSize.x);
        bool openPopup = ImGui::ArrowButton((std::string("##open_") + label).c_str(), ImGuiDir_Right);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(4);
        ImGui::PopItemWidth();

        if (clicked) *value = !*value;

        std::string popupName = std::string("##") + label;
        if (openPopup)
            ImGui::OpenPopup(popupName.c_str());

        auto scale = config::getGlobal<float>("UIScale");
        ImGui::SetNextWindowSizeConstraints(ImVec2(minWidth * scale, 0), ImVec2(FLT_MAX, FLT_MAX));
        if (ImGui::BeginPopup(popupName.c_str())) {
            popupDraw();
            ImGui::EndPopup();
        }

        return clicked;
    }

    void ModernTheme::popupSettings(const char *label, const std::function<void()> &content, ImVec2 size) {
        ImGui::PushItemWidth(-1.0f);

        if (utils::isSearching() && !utils::isSearched(label)) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
        }

        auto availWidth = ImGui::GetContentRegionAvail().x;
        auto buttonSize = ImVec2(availWidth * 0.88f, 0);
        auto arrowSize = ImVec2(availWidth * 0.12f, 0);

        if (size.x > 0) {
            buttonSize.x *= size.x;
            arrowSize.x *= size.x;
        }

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
        ImGui::Button(label, buttonSize);
        ImGui::PopStyleColor(4);
        ImGui::SameLine(0, 0);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.1f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.2f));
        ImGui::SetNextItemWidth(arrowSize.x);
        bool openPopup = ImGui::ArrowButton((std::string("##open_") + label).c_str(), ImGuiDir_Right);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);
        ImGui::PopItemWidth();

        std::string popupName = std::string("##") + label;
        if (openPopup)
            ImGui::OpenPopup(popupName.c_str());

        if (ImGui::BeginPopup(popupName.c_str())) {
            content();
            ImGui::EndPopup();
        }

    }

}