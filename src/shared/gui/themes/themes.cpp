#include "themes.hpp"

#include "classic/classic.hpp"
#include "megahack/megahack.hpp"
// #include "modern/modern.hpp"
// #include "steam2000/steam2000.hpp"

namespace openhack::gui {
    bool Theme::beginWindow(const char *name, bool *open, ImGuiWindowFlags flags) {
        return ImGui::Begin(name, open, flags);
    }

    void Theme::endWindow() {
        ImGui::End();
    }

    void Theme::text(const char *text, va_list args) {
        ImGui::TextV(text, args);
    }

    bool Theme::button(const char *label, const ImVec2 &size) {
        // if size is 0, the button will take the full width
        if (size.x == 0)
            ImGui::PushItemWidth(-1);

        auto availWidth = ImGui::GetContentRegionAvail().x;
        if (size.x > 0)
            availWidth *= size.x;
        bool clicked = ImGui::Button(label, ImVec2(availWidth, size.y));

        if (size.x == 0)
            ImGui::PopItemWidth();

        return clicked;
    }

    bool Theme::checkbox(const char *label, bool *value) {
        return ImGui::Checkbox(label, value);
    }

    bool Theme::combo(const char *label, int *current, const char *const items[], int itemsCount) {
        return ImGui::Combo(label, current, items, itemsCount);
    }

    bool Theme::combo(const char *label, int *current, const char *items) {
        return ImGui::Combo(label, current, items);
    }

    bool Theme::inputFloat(const char *label, float *value, float min, float max, const char *format) {
        bool changed = ImGui::InputFloat(label, value, 0.0f, 0.0f, format);
        if (*value < min)
            *value = min;
        if (*value > max)
            *value = max;
        return changed;
    }

    bool Theme::colorEdit(const char *label, Color *color, ImGuiColorEditFlags flags) {
        return ImGui::ColorEdit4(label, color->data(), flags);
    }

    void Theme::popupSettings(const char *label, const std::function<void()> &content, ImVec2 size) {
        // 95% is taken by a transparent button with label
        // 5% is taken by the small button with the arrow
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

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
        ImGui::SetNextItemWidth(arrowSize.x);
        bool openPopup = ImGui::ArrowButton("##", ImGuiDir_Right);
        ImGui::PopItemWidth();

        std::string popupName = std::string("##") + label;
        if (openPopup)
            ImGui::OpenPopup(popupName.c_str());

        if (ImGui::BeginPopup(popupName.c_str())) {
            content();
            ImGui::EndPopup();
        }
    }

    Theme *currentTheme = nullptr;

    Theme *getTheme() { return currentTheme; }

    void setTheme(Theme *theme) {
        delete currentTheme;
        currentTheme = theme;
    }

    void setTheme(Themes theme) {
        switch (theme) {
            case Themes::Classic:
                setTheme(new ClassicTheme());
                break;
            case Themes::MegaHack:
                setTheme(new MegaHackTheme());
                break;
        }
    }

    void setStyles() {
        if (currentTheme)
            currentTheme->setStyles();
    }

    void loadPalette() {
        if (currentTheme)
            currentTheme->loadPalette();
    }

    bool beginWindow(const char *name, bool *open, ImGuiWindowFlags flags) {
        if (currentTheme)
            return currentTheme->beginWindow(name, open, flags);
        return false;
    }

    void endWindow() {
        if (currentTheme)
            currentTheme->endWindow();
    }

    void text(const char *text, ...) {
        va_list args;
                va_start(args, text);
        if (currentTheme)
            currentTheme->text(text, args);
                va_end(args);
    }

    bool button(const char *label, const ImVec2 &size) {
        if (currentTheme)
            return currentTheme->button(label, size);
        return false;
    }

    bool checkbox(const char *label, bool *value) {
        if (currentTheme)
            return currentTheme->checkbox(label, value);
        return false;
    }

    bool combo(const char *label, int *current, const char *const items[], int itemsCount) {
        if (currentTheme)
            return currentTheme->combo(label, current, items, itemsCount);
        return false;
    }

    bool combo(const char *label, int *current, const char *items) {
        if (currentTheme)
            return currentTheme->combo(label, current, items);
        return false;
    }

    bool inputFloat(const char *label, float *value, float min, float max, const char *format) {
        if (currentTheme)
            return currentTheme->inputFloat(label, value, min, max, format);
        return false;
    }

    bool colorEdit(const char *label, Color *color, ImGuiColorEditFlags flags) {
        if (currentTheme)
            return currentTheme->colorEdit(label, color, flags);
        return false;
    }

    void popupSettings(const char *label, const std::function<void()> &content, ImVec2 size) {
        if (currentTheme)
            currentTheme->popupSettings(label, content, size);
    }

}