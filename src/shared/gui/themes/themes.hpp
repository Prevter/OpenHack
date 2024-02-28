#pragma once
#include "../../openhack.hpp"
#include <imgui.h>
#include <string>
#include <functional>
#include "../color.hpp"

namespace openhack::gui
{
    /// @brief The interface for a theme.
    /// A theme has redefined methods for drawing components.
    class Theme
    {
    public:
        virtual ~Theme() = default;

        /* == Meta == */
        /// @return The name of the theme to be displayed in the settings.
        virtual std::string getName() = 0;
        /// @brief Sets the styles for the theme. Called every frame before drawing.
        virtual void setStyles() = 0;
        /// @brief Loads the default color palette.
        virtual void loadPalette() = 0;

        /* == Windows == */
        /// @brief Begins a new window.
        /// @param name The name of the window.
        /// @param open The pointer to the bool that will store the open state of the window.
        /// @param flags The flags for the window.
        /// @return True if the window is open.
        virtual bool beginWindow(const char *name, bool *open, ImGuiWindowFlags flags);

        /// @brief Ends the current window.
        virtual void endWindow();

        /* == Components == */
        /// @brief Draws text with formatting.
        /// @param text The text to draw.
        /// @param ... The arguments for the format string.
        virtual void text(const char *text, va_list args);

        /// @brief Draws a button.
        /// @param label The label of the button.
        /// @param size The size of the button.
        /// @return True if the button was clicked.
        virtual bool button(const char *label, const ImVec2 &size);

        /// @brief Draws a checkbox.
        /// @param label The label of the checkbox.
        /// @param value The value of the checkbox.
        /// @return True if the checkbox was clicked.
        virtual bool checkbox(const char *label, bool *value);

        /// @brief Draws a combo box.
        /// @param label The label of the combo box.
        /// @param current The current value of the combo box.
        /// @param items The items of the combo box.
        /// @param itemsCount The count of the items.
        /// @return True if the combo box was clicked.
        virtual bool combo(const char *label, int *current, const char *const items[], int itemsCount);

        /// @brief Draws a combo box.
        /// @param label The label of the combo box.
        /// @param current The current value of the combo box.
        /// @param items Null-terminated string with items separated by '\0'.
        /// @return True if the combo box was clicked.
        virtual bool combo(const char *label, int *current, const char *items);

        /// @brief Draws a float input.
        /// @param label The label of the input.
        /// @param value The value of the input.
        /// @param min The minimum value of the input.
        /// @param max The maximum value of the input.
        /// @param format The format of the input.
        /// @return True if the input was changed.
        virtual bool inputFloat(const char *label, float *value, float min, float max, const char *format);

        /// @brief Draws a text input.
        /// @param label The label of the input.
        /// @param value The value of the input.
        /// @param bufferSize The size of the buffer.
        /// @param placeholder The placeholder of the input.
        /// @return True if the input was changed.
        virtual bool inputText(const char *label, std::string *value, int bufferSize, const char *placeholder);

        /// @brief Draws a color input.
        /// @param label The label of the input.
        /// @param color The color of the input.
        /// @return True if the input was changed.
        virtual bool colorEdit(const char *label, Color *color, ImGuiColorEditFlags flags);

        /// @brief Draws label and a small button which opens a popup with the content.
        /// @param label The label of the popup.
        /// @param content Callback for the content of the popup.
        /// @param size The size of the popup.
        virtual void popupSettings(const char* label, const std::function<void()> &content, ImVec2 size);

        /// @brief Draws a keybind input.
        /// @param label The label of the input.
        /// @param key The key of the input.
        /// @param canDelete Whether the key can be deleted (show the delete button).
        /// @return True if delete button was clicked.
        virtual bool keybind(const char *label, uint32_t *key, bool canDelete);

        /// @brief Draws a checkbox with a popup button.
        /// @param label The label of the checkbox.
        /// @param value The value of the checkbox.
        /// @param popupDraw The callback for the content of the popup.
        /// @return True if the checkbox was clicked.
        virtual bool toggleSetting(const char *label, bool *value, const std::function<void()> &popupDraw, ImVec2 size, float minWidth);

        /// @brief Draws a progress bar.
        /// @param fraction The fraction of the progress bar.
        virtual void progressBar(float fraction);
    };

    /* == Meta == */
    /// @brief Available themes.
    enum class Themes
    {
        /// @brief Classic ImGui theme.
        Classic = 0,
        /// @brief Theme based on the MegaHack v8
        MegaHack = 1,
        /// @brief Modern UI theme.
        // Modern = 2,
        /// @brief Theme based on the old Steam UI.
        // Steam2000 = 3,
    };

    /// @brief The names of the themes.
    const char *const THEME_NAMES[] = {
        "Classic",
        "MegaHack",
        // "Modern",
        // "Steam2000",
    };

    /// @brief The count of the themes.
    const size_t THEME_COUNT = sizeof(THEME_NAMES) / sizeof(THEME_NAMES[0]);

    /// @brief Change the current theme.
    /// @param theme The new theme.
    void setTheme(Theme *theme);

    /// @brief Change the current theme.
    /// @param theme The new theme.
    void setTheme(Themes theme);

    /// @brief Get the current theme.
    /// @return The current theme.
    Theme *getTheme();

    /// @brief Sets the styles for the theme. Called every frame before drawing.
    /// @note Calls the `setStyles` method of the current theme.
    void setStyles();

    /// @brief Loads the default color palette.
    /// @note Calls the `loadPalette` method of the current theme.
    void loadPalette();

    /* == Windows == */
    /// @brief Begins a new window.
    /// @param name The name of the window.
    /// @param open The pointer to the bool that will store the open state of the window.
    /// @param flags The flags for the window.
    /// @return True if the window is open.
    /// @note Calls the `beginWindow` method of the current theme.
    bool beginWindow(const char *name, bool *open, ImGuiWindowFlags flags = 0);

    /// @brief Ends the current window.
    /// @note Calls the `endWindow` method of the current theme.
    void endWindow();

    /* == Components == */
    /// @brief Draws text with formatting.
    /// @param text The text to draw.
    /// @param ... The arguments for the format string.
    /// @note Calls the `text` method of the current theme.
    void text(const char *text, ...);

    /// @brief Draws a button.
    /// @param label The label of the button.
    /// @param size The size of the button.
    /// @return True if the button was clicked.
    /// @note Calls the `button` method of the current theme.
    bool button(const char *label, const ImVec2 &size = ImVec2(0, 0));

    /// @brief Draws a checkbox.
    /// @param label The label of the checkbox.
    /// @param value The value of the checkbox.
    /// @return True if the checkbox was clicked.
    /// @note Calls the `checkbox` method of the current theme.
    bool checkbox(const char *label, bool *value);

    /// @brief Draws a combo box.
    /// @param label The label of the combo box.
    /// @param current The current value of the combo box.
    /// @param items The items of the combo box.
    /// @param itemsCount The count of the items.
    /// @return True if the combo box was clicked.
    /// @note Calls the `combo` method of the current theme.
    bool combo(const char *label, int *current, const char *const items[], int itemsCount);

    /// @brief Draws a combo box.
    /// @param label The label of the combo box.
    /// @param current The current value of the combo box.
    /// @param items Null-terminated string with items separated by '\0'.
    /// @return True if the combo box was clicked.
    /// @note Calls the `combo` method of the current theme.
    bool combo(const char *label, int *current, const char *items);

    /// @brief Draws a float input.
    /// @param label The label of the input.
    /// @param value The value of the input.
    /// @param min The minimum value of the input.
    /// @param max The maximum value of the input.
    /// @param format The format of the input.
    /// @return True if the input was changed.
    /// @note Calls the `inputFloat` method of the current theme.
    bool inputFloat(const char *label, float *value, float min = -FLT_MAX, float max = FLT_MAX, const char *format = "%.3f");

    /// @brief Draws a text input.
    /// @param label The label of the input.
    /// @param value The value of the input.
    /// @param bufferSize The size of the buffer.
    /// @param placeholder The placeholder of the input.
    /// @return True if the input was changed.
    /// @note Calls the `inputText` method of the current theme.
    bool inputText(const char *label, std::string *value, int bufferSize = 64, const char *placeholder = "");

    /// @brief Draws a color input.
    /// @param label The label of the input.
    /// @param color The color of the input.
    /// @return True if the input was changed.
    /// @note Calls the `colorEdit` method of the current theme.
    bool colorEdit(const char *label, Color *color, ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs);

    /// @brief Draws label and a small button which opens a popup with the content.
    /// @param label The label of the popup.
    /// @param content Callback for the content of the popup.
    /// @param size The size of the popup.
    /// @note Calls the `popupSettings` method of the current theme.
    void popupSettings(const char* label, const std::function<void()> &content, ImVec2 size = ImVec2(0, 0));

    /// @brief Draws a keybind input.
    /// @param label The label of the input.
    /// @param key The key of the input.
    /// @param canDelete Whether the key can be deleted (show the delete button).
    /// @return True if delete button was clicked.
    /// @note Calls the `keybind` method of the current theme.
    bool keybind(const char *label, uint32_t *key, bool canDelete = false);

    /// @brief Draws a checkbox with a popup button.
    /// @param label The label of the checkbox.
    /// @param value The value of the checkbox.
    /// @param popupDraw The callback for the content of the popup.
    /// @return True if the checkbox was clicked.
    /// @note Calls the `toggleSetting` method of the current theme.
    bool toggleSetting(const char *label, bool *value, const std::function<void()> &popupDraw, ImVec2 size = ImVec2(0, 0), float minWidth = 0.0f);

    /// @brief Draws a progress bar.
    /// @param fraction The fraction of the progress bar.
    /// @note Calls the `progressBar` method of the current theme.
    void progressBar(float fraction);
}