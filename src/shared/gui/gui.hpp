#pragma once

#include <utility>

#include "themes/themes.hpp"
#include "window.hpp"

namespace openhack::gui {
    /// @brief Struct containing variants of one font.
    struct Font {
        /// @brief The name of the font.
        std::string name;
        /// @brief The normal variant of the font (16px)
        ImFont *normal;
        /// @brief Larger variant of the font (20px)
        ImFont *title;

        /// @brief Construct a new Font object.
        /// @param name The name of the font.
        /// @param normal The normal variant of the font.
        /// @param title The larger variant of the font.
        Font(std::string name, ImFont *normal, ImFont *title)
                : name(std::move(name)), normal(normal), title(title) {}
    };

    /// @brief Get currently selected font.
    Font &getFont();

    /// @brief Get all available fonts.
    std::vector<Font> &getFonts();

    /// @brief Get the index of the currently selected font.
    int32_t getFontIndex();

    /// @brief Set the font to use.
    /// @param font The font to use.
    void setFont(Font *font);

    /// @brief Set the font to use.
    /// @param name The name of the font to use.
    bool setFont(const std::string &name);

    /// @brief Callback for when the GUI is initialized.
    void init();

    /// @brief ImGui::PushItemWidth and ImGui::PopItemWidth wrapper.
    /// @param width The width to push. (0.0f for default)
    void width(float width = 0.0f);

    /// @brief Calculate the width using proportional size of an active container.
    /// @param f The factor to multiply the width by.
    /// @return The width.
    float factor(float f);

    /// @brief ImGui::PushItemWidth and ImGui::PopItemWidth wrapper. Uses proportions.
    /// @param f Value from 0 to 1 to multiply the width by.
    /// @return The width to push.
    void widthF(float f);

    /// @brief Draws a tooltip.
    /// @param text The text of the tooltip.
    void tooltip(const char *text);

    /// @brief Draws a checkbox.
    /// @param label The label of the checkbox.
    /// @param valueKey The key of the value in the configuration.
    /// @return True if the checkbox was clicked.
    /// @note Uses the configuration to store the value.
    bool checkbox(const char *label, const char *valueKey);

    /// @brief Draws a combo box.
    /// @param label The label of the combo box.
    /// @param current The key of the current value in the configuration.
    /// @param items The items of the combo box.
    /// @param itemsCount The count of the items.
    /// @return True if the combo box was clicked.
    /// @note Uses the configuration to store the value.
    bool combo(const char *label, const char *current, const char *const items[], int itemsCount);

    /// @brief Draws a combo box.
    /// @param label The label of the combo box.
    /// @param current The current value of the combo box.
    /// @param items Null-terminated string with items separated by '\0'.
    /// @return True if the combo box was clicked.
    /// @note Uses the configuration to store the value.
    bool combo(const char *label, const char *current, const char *items);

    /// @brief Draws a float input.
    /// @param label The label of the input.
    /// @param valueKey The key of the value in the configuration.
    /// @param step The step of the input.
    /// @param stepFast The fast step of the input.
    /// @return True if the input was changed.
    /// @note Uses the configuration to store the value.
    bool inputFloat(const char *label, const char *valueKey, float min = -FLT_MAX, float max = FLT_MAX,
                    const char *format = "%.3f");

    /// @brief Draws a text input.
    /// @param label The label of the input.
    /// @param valueKey The key of the value in the configuration.
    /// @param bufferSize The size of the buffer.
    /// @param placeholder The placeholder of the input.
    /// @return True if the input was changed.
    /// @note Uses the configuration to store the value.
    bool inputText(const char *label, const char *valueKey, int bufferSize = 64, const char *placeholder = "");

    /// @brief Draws a color input.
    /// @param label The label of the input.
    /// @param color The key of the color in the configuration.
    /// @return True if the input was changed.
    /// @note Uses the configuration to store the value.
    bool colorEdit(const char *label, const char *colorKey);

    /// @brief Draws a keybind input.
    /// @param label The label of the input.
    /// @param key The key of the input.
    /// @param canDelete Whether the key can be deleted (show the delete button).
    /// @return True if delete button was clicked.
    /// @note Uses the configuration to store the value.
    bool keybind(const char *label, const char *keybindKey, bool canDelete = false);

    /// @brief Draws a checkbox with a popup button.
    /// @param label The label of the checkbox.
    /// @param settingKey The key of the value in the configuration.
    /// @param popupDraw The callback for the content of the popup.
    /// @param size The size of the toggle button.
    /// @param minWidth The minimum width of the popup.
    /// @return True if the checkbox was clicked.
    /// @note Uses the configuration to store the value.
    bool toggleSetting(const char *label, const char *settingKey, std::function<void()> popupDraw,
                       ImVec2 size = ImVec2(0, 0), float minWidth = 0.0f);
}