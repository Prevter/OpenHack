#include "themes.hpp"

#include "classic/classic.hpp"
#include "megahack/megahack.hpp"
// #include "modern/modern.hpp"
// #include "steam2000/steam2000.hpp"

namespace openhack::gui {
    bool Theme::beginWindow(const char *name, bool *open, ImGuiWindowFlags flags) {
        auto font = gui::getFont();
        ImGui::PushFont(font.title);
        bool opened = ImGui::Begin(name, open, flags);
        ImGui::PushFont(font.normal);
        auto scale = config::getGlobal<float>("UIScale");
        ImGui::SetWindowFontScale(scale);
        return opened;
    }

    void Theme::endWindow() {
        ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
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
        if (utils::isSearching()) {
            if (utils::isSearched(label)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
            }
        } else {
            auto &style = ImGui::GetStyle();
            ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_Text]);
        }
        bool changed = ImGui::Checkbox(label, value);
        ImGui::PopStyleColor(4);
        return changed;
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

    bool Theme::inputText(const char *label, std::string *value, int bufferSize, const char *placeholder, ImGuiTextFlags flags) {
        char buffer[256];
        std::copy(value->begin(), value->end(), buffer);
        buffer[value->size()] = '\0';
        bool changed;
        if (placeholder != nullptr) {
            changed = ImGui::InputTextWithHint(label, placeholder, buffer, bufferSize, flags);
        } else {
            changed = ImGui::InputText(label, buffer, bufferSize, flags);
        }
        *value = buffer;
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

    bool Theme::keybind(const char *label, uint32_t *key, bool canDelete) {
        ImGui::PushID(label);
        ImGui::PushItemWidth(-1);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

        auto availWidth = ImGui::GetContentRegionAvail().x;

        // Truncate label to fit in the width
        auto labelMaxWidth = availWidth * (canDelete ? 0.5f : 0.6f);
        auto labelSize = ImGui::CalcTextSize(label);

        if (labelSize.x > labelMaxWidth) {
            auto labelEnd = label;
            while (labelEnd != label + strlen(label)) {
                auto labelStr = std::string(label, labelEnd) + "...";
                auto newSize = ImGui::CalcTextSize(labelStr.c_str());
                if (newSize.x > labelMaxWidth - 20)
                    break;
                labelEnd++;
            }
            auto truncatedLabel = std::string(label, labelEnd) + "...";
            ImGui::Button(truncatedLabel.c_str(), ImVec2(labelMaxWidth, 0));
            gui::tooltip(label);
        } else {
            ImGui::Button(label, ImVec2(labelMaxWidth, 0));
        }

        ImGui::SameLine(0, 0);

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0.25f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

        auto keyName = utils::getKeyName(*key);
        bool changed = ImGui::Button(keyName.c_str(), ImVec2(availWidth * 0.4f, 0));
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        auto popupName = std::string("##") + label;
        if (changed)
            ImGui::OpenPopup(popupName.c_str());

        if (ImGui::BeginPopup(popupName.c_str())) {
            utils::lockTickInput();
            gui::text("Press any key to change the keybind...");
            ImGui::Separator();

            gui::text("Press ESC to clear the cancel.");

            if (utils::isKeyPressed("Esc")) {
                ImGui::CloseCurrentPopup();
            } else {
                for (uint32_t i = 0; i < 256; i++) {
                    if (utils::isKeyPressed(i)) {
                        *key = i;
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }
            }

            ImGui::EndPopup();
        }

        bool deleteClicked = false;
        if (canDelete) {
            ImGui::SameLine(0, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
            deleteClicked = ImGui::Button("X", ImVec2(availWidth * 0.1f, 0));
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();
            if (deleteClicked) {
                *key = 0;
            }
        }

        ImGui::PopItemWidth();
        ImGui::PopID();
        return deleteClicked;
    }

    bool Theme::toggleSetting(const char *label, bool *value, const std::function<void()> &popupDraw, ImVec2 size,
                              float minWidth) {
        // 95% is taken by a transparent button with label
        // 5% is taken by the small button with the arrow
        ImGui::PushItemWidth(-1);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

        if (utils::isSearching()) {
            if (utils::isSearched(label)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
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
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
        }

        auto availWidth = ImGui::GetContentRegionAvail().x;
        auto buttonSize = ImVec2(availWidth * 0.9f, 0);
        auto arrowSize = ImVec2(availWidth * 0.1f, 0);

        if (size.x > 0) {
            buttonSize.x *= size.x;
            arrowSize.x *= size.x;
        }

        bool changed = ImGui::Checkbox(label, value);

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);
        gui::callback();

        ImGui::SameLine(0, 0);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - arrowSize.x);
        ImGui::SetNextItemWidth(arrowSize.x);
        bool openPopup = ImGui::ArrowButton((std::string("##open_") + label).c_str(), ImGuiDir_Right);
        ImGui::PopItemWidth();

        std::string popupName = std::string("##") + label;
        if (openPopup) {
            ImGui::OpenPopup(popupName.c_str());
        }

        auto scale = config::getGlobal<float>("UIScale");
        ImGui::SetNextWindowSizeConstraints(ImVec2(minWidth * scale, 0), ImVec2(FLT_MAX, FLT_MAX));
        if (ImGui::BeginPopup(popupName.c_str())) {
            popupDraw();
            ImGui::EndPopup();
        }

        return changed;
    }

    void Theme::progressBar(float progress) {
        auto label = fmt::format("{:.0f}%", progress * 100);
        ImGui::ProgressBar(progress, ImVec2(-1, 0), label.c_str());
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

    bool inputText(const char *label, std::string *buffer, int bufferSize, const char *placeholder, ImGuiTextFlags flags) {
        if (currentTheme)
            return currentTheme->inputText(label, buffer, bufferSize, placeholder, flags);
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

    bool keybind(const char *label, uint32_t *key, bool canDelete) {
        if (currentTheme)
            return currentTheme->keybind(label, key, canDelete);
        return false;
    }

    bool
    toggleSetting(const char *label, bool *value, const std::function<void()> &popupDraw, ImVec2 size, float minWidth) {
        if (currentTheme)
            return currentTheme->toggleSetting(label, value, popupDraw, size, minWidth);
        return false;
    }

    void progressBar(float progress) {
        if (currentTheme)
            currentTheme->progressBar(progress);
    }

}