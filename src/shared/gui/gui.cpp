#include "gui.hpp"
#include "../openhack.hpp"
#include <imgui.h>

namespace openhack::gui {
    std::vector<Font> fonts;
    Font *currentFont = nullptr;

    void setFont(Font *font) {
        currentFont = font;
        ImGui::GetIO().FontDefault = font->normal;
    }

    bool setFont(const std::string &name) {
        for (auto &font: fonts) {
            if (font.name == name) {
                setFont(&font);
                config::set("menu.font", name);
                return true;
            }
        }

        L_ERROR("Font not found: {}", name);
        return false;
    }

    Font &getFont() {
        return *currentFont;
    }

    std::vector<Font> &getFonts() {
        return fonts;
    }

    int32_t getFontIndex() {
        for (int32_t i = 0; i < fonts.size(); i++) {
            if (&fonts[i] == currentFont)
                return i;
        }
        return 0;
    }

    /// @brief Verify that all colors are stored in the config
    /// @return True if all colors are stored in the config
    bool checkPalette() {
        return config::has("menu.color.text") &&
               config::has("menu.color.textDisabled") &&
               config::has("menu.color.background") &&
               config::has("menu.color.accent") &&
               config::has("menu.color.primary") &&
               config::has("menu.color.secondary") &&
               config::has("menu.color.border") &&
               config::has("menu.color.hovered") &&
               config::has("menu.color.clicked");
    }

    void init() {
        utils::resetWindowHandle();

        // Load all fonts from "OPENHACK_DIRECTORY/fonts/"
        auto fontDir = utils::getModFontsDirectory();

        // Check if the directory exists
        if (!std::filesystem::exists(fontDir)) {
            L_ERROR("Font directory does not exist: {}", fontDir.string());
            return;
        }

        ImGuiIO &io = ImGui::GetIO();

        io.IniFilename = nullptr;

        // Scan for fonts
        ImFontConfig font_cfg;
        font_cfg.OversampleH = 3;
        font_cfg.OversampleV = 3;

        fonts.clear();
        auto fontSize = config::get<float>("menu.fontSize", 16.0f);
        for (const auto &entry: std::filesystem::directory_iterator(fontDir)) {
            if (entry.is_regular_file()) {
                auto path = entry.path().string();
                auto ext = path.substr(path.find_last_of('.') + 1);
                if (ext == "ttf") {
                    auto fontDefault = io.Fonts->AddFontFromFileTTF(path.c_str(), fontSize, &font_cfg);
                    auto fontTitle = io.Fonts->AddFontFromFileTTF(path.c_str(), fontSize + 4.0f, &font_cfg);
                    if (fontDefault && fontTitle) {
                        auto name = entry.path().stem().string();
                        name = name.substr(0, name.find_last_of('.'));
                        auto font = Font(name, fontDefault, fontTitle);
                        fonts.push_back(font);
                    } else {
                        L_ERROR("Failed to load font: {}", path);
                    }
                }
            }
        }

        // Set default font
        if (!fonts.empty()) {
            auto selectedFont = config::get<std::string>("menu.font", "Poppins");
            if (!setFont(selectedFont)) {
                auto first = fonts.front();
                L_TRACE("Setting default font: {}", first.name);
                setFont(first.name);
            }
        }

        // Set theme
        auto theme = config::get<Themes>("menu.theme");
        setTheme(theme);

        if (!checkPalette())
            loadPalette();

        L_INFO("Initialized ImGui");
    }

    void width(float width) {
        if (width > 0) {
            auto scale = config::getGlobal<float>("UIScale");
            ImGui::PushItemWidth(width * scale);
        } else {
            ImGui::PopItemWidth();
        }
    }

    float factor(float f) {
        auto availWidth = ImGui::GetContentRegionAvail().x;
        return availWidth * f;
    }

    void widthF(float f) {
        ImGui::PushItemWidth(factor(f));
    }

    void tooltip(const char *text) {
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImVec2 pos = ImGui::GetMousePos();

            auto size = ImGui::CalcTextSize(text);
            size.x /= config::getGlobal<float>("UIScale");
            auto screenSize = ImGui::GetIO().DisplaySize;

            const float padding = 10.0f;
            pos.x += padding;
            pos.y += padding;

            if (pos.x + size.x > screenSize.x)
                pos.x = screenSize.x - size.x - padding;
            if (pos.y + size.y > screenSize.y)
                pos.y = screenSize.y - size.y - padding;

            ImGui::SetNextWindowPos(pos);
            ImGui::SetTooltip("%s", text);
        }
    }

    bool checkbox(const char *label, const char *valueKey) {
        bool value = config::get<bool>(valueKey, false);
        bool result = gui::checkbox(label, &value);
        if (result)
            config::set(valueKey, value);
        return result;
    }

    bool combo(const char *label, const char *current, const char *const items[], int itemsCount) {
        int value = config::get<int>(current, 0);
        bool result = gui::combo(label, &value, items, itemsCount);
        if (result)
            config::set(current, value);
        return result;
    }

    bool combo(const char *label, const char *current, const char *items) {
        int value = config::get<int>(current, 0);
        bool result = gui::combo(label, &value, items);
        if (result)
            config::set(current, value);
        return result;
    }

    bool inputFloat(const char *label, const char *valueKey, float min, float max, const char *format) {
        auto value = config::get<float>(valueKey, 0.0f);
        bool result = gui::inputFloat(label, &value, min, max, format);
        if (result)
            config::set(valueKey, value);
        return result;
    }

    bool inputText(const char *label, const char *valueKey, int bufferSize, const char *placeholder) {
        auto value = config::get<std::string>(valueKey, "");
        bool result = gui::inputText(label, &value, bufferSize, placeholder);
        if (result)
            config::set(valueKey, value);
        return result;
    }

    bool colorEdit(const char *label, const char *colorKey) {
        auto value = config::get<Color>(colorKey, Color());
        bool result = gui::colorEdit(label, &value);
        if (result)
            config::set(colorKey, value);
        return result;
    }

    bool keybind(const char *label, const char *keybindKey, bool canDelete) {
        auto keyName = config::get<std::string>(keybindKey, "...");
        auto value = utils::getKeyCode(keyName);
        auto original = value;
        bool result = gui::keybind(label, &value, canDelete);
        if (value != original)
            config::set<std::string>(keybindKey, utils::getKeyName(value));
        return result;
    }

    bool toggleSetting(const char *label, const char *settingKey, std::function<void()> popupDraw, ImVec2 size, float minWidth) {
        bool value = config::get<bool>(settingKey, false);
        bool result = gui::toggleSetting(label, &value, popupDraw, size, minWidth);
        if (result)
            config::set(settingKey, value);
        return result;
    }

    static std::function<void()> s_currentCallback = nullptr;

    void callback(const std::function<void()> &callback) {
        if (callback == nullptr && s_currentCallback != nullptr) {
            s_currentCallback();
            s_currentCallback = nullptr;
        } else {
            s_currentCallback = callback;
        }
    }
}