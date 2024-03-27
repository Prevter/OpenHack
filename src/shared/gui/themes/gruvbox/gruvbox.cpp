#include "gruvbox.hpp"

namespace openhack::gui {

    void GruvboxTheme::setStyles() {
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
        style.ScaleAllSizes(config::getGlobal<float>("UIScale"));
        style.WindowMenuButtonPosition = ImGuiDir_Right;

        auto &colors = style.Colors;
        colors[ImGuiCol_Text] = config::get<Color>("menu.color.text");
        colors[ImGuiCol_TextDisabled] = config::get<Color>("menu.color.textDisabled");

        colors[ImGuiCol_WindowBg] = config::get<Color>("menu.color.background");
        colors[ImGuiCol_FrameBg] = config::get<Color>("menu.color.secondary");
        colors[ImGuiCol_TitleBg] =
        colors[ImGuiCol_TitleBgActive] =
        colors[ImGuiCol_TitleBgCollapsed] =
                config::get<Color>(config::get<bool>("menu.rainbow.enabled", false) ? "menu.color.accent"
                                                                                    : "menu.color.background");

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

    void GruvboxTheme::loadPalette() {
        config::set("menu.color.text", Color(0.92f, 0.86f, 0.7f, 1.0f)); // Text color
        config::set("menu.color.textDisabled", Color(0.51f, 0.65f, 0.6f, 0.95f)); // Disabled text color
        config::set("menu.color.background", Color(0.24f, 0.22f, 0.21f, 0.999f)); // Window background
        config::set("menu.color.accent", Color(0.4f, 0.62f, 0.41f, 1.0f)); // Title background
        config::set("menu.color.primary", Color(0.98f, 0.29f, 0.20f, 1.0f)); // Button color
        config::set("menu.color.secondary", Color(0.18f, 0.16f, 0.15f, 0.95f)); // Frame background
        config::set("menu.color.border", Color(0.66f, 0.6f, 0.52f, 1.0f)); // Border color
        config::set("menu.color.hovered", Color(0.961f, 0.357f, 0.282f, 1.0f)); // Hovered color
        config::set("menu.color.clicked", Color(0.95f, 0.42f, 0.36f, 1.0f)); // Clicked color

        config::set("menu.windowRounding", 0.0f);
        config::set("menu.frameRounding", 7.0f);
        config::set("menu.borderSize", 1.0f);
    }

    bool GruvboxCheckbox(const char *label, bool value, float width = -1.0f) {
        auto *drawList = ImGui::GetWindowDrawList();
        auto pos = ImGui::GetCursorScreenPos();
        auto scale = config::getGlobal<float>("UIScale");
        auto padding = scale * 3;
        auto size = ImGui::GetFrameHeight() - padding * 2;
        auto frameColor = ImGui::GetColorU32(ImGuiCol_FrameBg);

        auto start = ImVec2(pos.x + padding, pos.y + padding);
        auto end = ImVec2(start.x + size, start.y + size);

        drawList->AddRectFilled(start, end, frameColor, scale * 6);
        if (value) {
            auto checkColor = ImGui::GetColorU32(ImGuiCol_CheckMark);
            auto checkStart = ImVec2(start.x + 3, start.y + 3);
            auto checkEnd = ImVec2(end.x - 3, end.y - 3);
            drawList->AddRectFilled(checkStart, checkEnd, checkColor, scale * 3);
        }

        auto leftMargin = size + padding * 2;
        auto btnWidth = width > 0 ? width : ImGui::GetContentRegionAvail().x;
        bool clicked = ImGui::Button(fmt::format("##{}", label).c_str(), ImVec2(btnWidth, 0));
        auto buttonPos = ImGui::GetItemRectMin();
        auto text = std::string(label);
        auto hashPos = text.find("##");
        if (hashPos != std::string::npos)
            text = text.substr(0, hashPos);
        drawList->AddText(ImVec2(buttonPos.x + leftMargin, buttonPos.y + padding / 2),
                          ImGui::GetColorU32(ImGuiCol_Text), text.c_str());

        return clicked;
    }

    bool GruvboxTheme::checkbox(const char *label, bool *value) {
        ImGui::PushItemWidth(-1.0f);
        auto textColor = *value ? config::get<Color>("menu.color.accent") : config::get<Color>(
                "menu.color.textDisabled");

        if (utils::isSearching()) {
            if (utils::isSearched(label)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_CheckMark, (ImVec4) config::get<Color>("menu.color.primary"));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.1f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.2f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) textColor);
            ImGui::PushStyleColor(ImGuiCol_CheckMark, (ImVec4) textColor);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.1f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.2f));
        }

        bool clicked = GruvboxCheckbox(label, *value);
        ImGui::PopStyleColor(5);

        if (clicked) *value = !*value;
        ImGui::PopItemWidth();
        return clicked;
    }

    bool GruvboxTheme::toggleSetting(const char *label, bool *value, const std::function<void()> &popupDraw, ImVec2 size,
                                    float minWidth) {
        ImGui::PushItemWidth(-1.0f);
        auto textColor = *value ? config::get<Color>("menu.color.accent") : config::get<Color>(
                "menu.color.textDisabled");

        if (utils::isSearching()) {
            if (utils::isSearched(label)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_CheckMark, (ImVec4) config::get<Color>("menu.color.primary"));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.1f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.2f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) textColor);
            ImGui::PushStyleColor(ImGuiCol_CheckMark, (ImVec4) textColor);
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

        bool clicked = GruvboxCheckbox(label, *value, buttonSize.x);
        ImGui::PopStyleColor(4);
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) config::get<Color>("menu.color.text"));
        gui::callback();
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 0);
        ImGui::SetNextItemWidth(arrowSize.x);
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) config::get<Color>("menu.color.text"));
        bool openPopup = ImGui::ArrowButton((std::string("##open_") + label).c_str(), ImGuiDir_Right);
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(2);
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

    void GruvboxTheme::popupSettings(const char *label, const std::function<void()> &content, ImVec2 size) {
        ImGui::PushItemWidth(-1);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

        auto availWidth = ImGui::GetContentRegionAvail().x;
        auto buttonSize = ImVec2(availWidth * 0.885f, 0);
        auto arrowSize = ImVec2(availWidth * 0.115f, 0);

        if (size.x > 0) {
            buttonSize.x *= size.x;
            arrowSize.x *= size.x;
        }

        ImGui::Button(label, buttonSize);
        ImGui::SameLine(0, 0);

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
        ImGui::SetNextItemWidth(arrowSize.x);
        bool openPopup = ImGui::ArrowButton((std::string("##open_") + label).c_str(), ImGuiDir_Right);
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