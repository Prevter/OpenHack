#include "megahack.hpp"

namespace openhack::gui {

    void MegaHackTheme::setStyles() {
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

    void MegaHackTheme::loadPalette() {
        config::set("menu.color.text", Color(1.0f, 1.0f, 1.0f, 1.0f)); // Text color
        config::set("menu.color.textDisabled", Color(0.49f, 0.5f, 0.5f, 1.0f)); // Disabled text color
        config::set("menu.color.background", Color(0.16f, 0.16f, 0.16f, 1.0f)); // Window background
        config::set("menu.color.accent", Color(0.3f, 0.75f, 0.61f, 1.0f)); // Title background
        config::set("menu.color.primary", Color(0.11f, 0.11f, 0.11f, 1.0f)); // Buttons, etc.
        config::set("menu.color.secondary", Color(0.13f, 0.13f, 0.13f, 1.0f)); // Frame background
        config::set("menu.color.border", Color(0.0f, 0.0f, 0.0f, 1.0f)); // Border color
        config::set("menu.color.hovered", Color(0.3f, 0.76f, 0.6f, 1.0f)); // Item hovered
        config::set("menu.color.clicked", Color(0.22f, 0.55f, 0.44f, 1.0f)); // Item clicked
    }

    bool MegaHackTheme::button(const char *label, const ImVec2 &size) {
        // if size is 0, the button will take the full width
        if (size.x == 0)
            ImGui::PushItemWidth(-1);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

        auto availWidth = ImGui::GetContentRegionAvail().x;
        if (size.x > 0)
            availWidth *= size.x;

        // auto cursorLeft = ImGui::GetCursorPos().x;
        // auto cursorRight = cursorLeft + availWidth;

        bool clicked = ImGui::Button(label, ImVec2(availWidth, size.y));

        // Draw two lines
        bool isMouseOver = ImGui::IsItemHovered();
        bool isItemActive = ImGui::IsItemActive();

        Color color;
        if (isMouseOver) {
            if (isItemActive) {
                color = config::get<Color>("menu.color.clicked");
            } else {
                color = config::get<Color>("menu.color.hovered");
            }
        } else {
            color = config::get<Color>("menu.color.primary");
        }

        auto scale = config::get<float>("menu.uiScale");

        ImGui::GetWindowDrawList()->AddLine(
                ImVec2(ImGui::GetItemRectMin().x + 1, ImGui::GetItemRectMin().y + 1),
                ImVec2(ImGui::GetItemRectMin().x + 1, ImGui::GetItemRectMax().y - 3),
                color, 2.5f * scale);

        ImGui::GetWindowDrawList()->AddLine(
                ImVec2(ImGui::GetItemRectMax().x - 2, ImGui::GetItemRectMin().y + 1),
                ImVec2(ImGui::GetItemRectMax().x - 2, ImGui::GetItemRectMax().y - 3),
                color, 2.5f * scale);

        ImGui::PopStyleColor(3);

        if (size.x == 0)
            ImGui::PopItemWidth();

        return clicked;
    }

    bool MegaHackTheme::checkbox(const char *label, bool *value) {
        ImGui::PushItemWidth(-1.0f);

        if (utils::isSearching()) {
            if (utils::isSearched(label)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            }
        } else {
            auto &style = ImGui::GetStyle();
            ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_Text]);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

        auto textColor = *value ? config::get<Color>("menu.color.text") : config::get<Color>("menu.color.textDisabled");
        if (!utils::isSearching())
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) textColor);

        bool clicked = ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x, 0));
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();

        auto scale = config::get<float>("menu.uiScale");

        ImGui::GetWindowDrawList()->AddRectFilled(
                ImVec2(ImGui::GetItemRectMax().x - 5 * scale, ImGui::GetItemRectMin().y + 1 * scale),
                ImVec2(ImGui::GetItemRectMax().x - 2 * scale, ImGui::GetItemRectMax().y - 1 * scale),
                textColor);

        if (clicked)
            *value = !*value;

        ImGui::PopItemWidth();
        return clicked;
    }

    void MegaHackTheme::popupSettings(const char *label, const std::function<void()> &content, ImVec2 size) {
        ImGui::PushItemWidth(-1);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

        auto availWidth = ImGui::GetContentRegionAvail().x;
        auto buttonSize = ImVec2(availWidth * 0.9f, 0);
        auto arrowSize = ImVec2(availWidth * 0.1f, 0);

        if (size.x > 0) {
            buttonSize.x *= size.x;
            arrowSize.x *= size.x;
        }

        ImGui::Button(label, buttonSize);
        ImGui::SameLine(0, 0);

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
        bool openPopup = ImGui::Button((std::string("##open_") + label).c_str(), arrowSize);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(2);

        auto scale = config::get<float>("menu.uiScale");
        auto top = ImGui::GetItemRectMin().y + (6 * scale);
        auto bottom = ImGui::GetItemRectMax().y - (6 * scale);
        auto right = ImGui::GetItemRectMax().x - (6 * scale);
        auto side = bottom - top;
        auto left = right - side;
        ImGui::GetWindowDrawList()->AddTriangleFilled(
                ImVec2(right, top),
                ImVec2(left, bottom),
                ImVec2(right, bottom),
                config::get<Color>("menu.color.textDisabled"));

        std::string popupName = std::string("##") + label;
        if (openPopup)
            ImGui::OpenPopup(popupName.c_str());

        if (ImGui::BeginPopup(popupName.c_str())) {
            content();
            ImGui::EndPopup();
        }
    }

    bool
    MegaHackTheme::toggleSetting(const char *label, bool *value, const std::function<void()> &popupDraw, ImVec2 size,
                                 float minWidth) {
        ImGui::PushItemWidth(-1);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

        auto color = *value ? config::get<Color>("menu.color.text") : config::get<Color>("menu.color.textDisabled");

        if (utils::isSearching()) {
            if (utils::isSearched(label)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) color);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
        }

        auto availWidth = ImGui::GetContentRegionAvail().x;
        auto buttonSize = ImVec2(availWidth * 0.9f, 0);
        auto arrowSize = ImVec2(availWidth * 0.1f, 0);

        if (size.x > 0) {
            buttonSize.x *= size.x;
            arrowSize.x *= size.x;
        }

        bool toggled = ImGui::Button(label, buttonSize);
        ImGui::SameLine(0, 0);

        if (toggled) {
            *value = !*value;
        }

        ImGui::PopStyleVar(2);
        bool openPopup = ImGui::Button((std::string("##open_") + label).c_str(), arrowSize);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(4);

        auto scale = config::get<float>("menu.uiScale");
        auto top = ImGui::GetItemRectMin().y + (6 * scale);
        auto bottom = ImGui::GetItemRectMax().y - (6 * scale);
        auto right = ImGui::GetItemRectMax().x - (6 * scale);
        auto side = bottom - top;
        auto left = right - side;
        ImGui::GetWindowDrawList()->AddTriangleFilled(
                ImVec2(right, top),
                ImVec2(left, bottom),
                ImVec2(right, bottom),
                color);

        std::string popupName = std::string("##") + label;
        if (openPopup)
            ImGui::OpenPopup(popupName.c_str());

        ImGui::SetNextWindowSizeConstraints(ImVec2(minWidth * scale, 0), ImVec2(FLT_MAX, FLT_MAX));
        if (ImGui::BeginPopup(popupName.c_str())) {
            popupDraw();
            ImGui::EndPopup();
        }

        return toggled;
    }

}