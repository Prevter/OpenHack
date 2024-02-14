#pragma once
#include <utility>

#include "themes/themes.hpp"
#include "window.hpp"

namespace openhack::gui
{
    /// @brief Struct containing variants of one font.
    struct Font
    {
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
    void setFont(const std::string &name);

    /// @brief Callback for when the GUI is initialized.
    void init();

    /// @brief ImGui::PushItemWidth and ImGui::PopItemWidth wrapper.
    /// @param width The width to push. (0.0f for default)
    void width(float width = 0.0f);

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
    bool inputFloat(const char *label, const char *valueKey, float min = -FLT_MAX, float max = FLT_MAX, const char *format = "%.3f");

    /// @brief Draws a color input.
    /// @param label The label of the input.
    /// @param color The key of the color in the configuration.
    /// @return True if the input was changed.
    /// @note Uses the configuration to store the value.
    bool colorEdit(const char *label, const char *colorKey);
}