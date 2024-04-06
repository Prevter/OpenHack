#pragma once

#include <string>
#include <functional>

#if OPENHACK_EXPORT
    #define OPENHACK_DLL __declspec(dllexport)
#else
    #define OPENHACK_DLL __declspec(dllimport)
#endif

#ifdef GEODE_IS_WINDOWS
#define LOG_ERROR(message) geode::log::error(message)
#else
#include <iostream>
#define LOG_ERROR(message) std::cerr << message << std::endl
#endif

#ifdef OPENHACK_OPTIONAL
// Call the methods directly from the DLL.
#ifdef GEODE_IS_WINDOWS
#define OPENHACK_DLL_NAME "prevter.openhack.dll"
#else
#define OPENHACK_DLL_NAME "xinput9_1_0.dll"
#endif

#define CREATE_OPENHACK_DLL_METHOD(signature, callingConvention, methodBody)  \
    auto library = GetModuleHandleA(OPENHACK_DLL_NAME);           \
    if (library) {                                                \
        auto method = reinterpret_cast<callingConvention>(GetProcAddress(library, signature)); \
        methodBody                                                \
    }                                                             \
    LOG_ERROR("OpenHack is not loaded into the game!")    \

namespace openhack {
    /// @brief Check if OpenHack is loaded into the game.
    /// Should be used to check if the DLL is loaded before calling any methods.
    inline bool isLoaded() {
        auto library = GetModuleHandleA(OPENHACK_DLL_NAME);
        return library != nullptr;
    }

    /// @brief Check whether any "cheats" are enabled (Those that trigger Cheat Indicator/Auto Safe Mode).
    inline bool isCheating() {
        CREATE_OPENHACK_DLL_METHOD("?isCheating@openhack@@YA_NXZ", bool (*)(), {
            return method();
        });
        return false;
    }

    /// @brief Get the value of a setting stored in OpenHack's configuration.
    /// @param key The key of the setting.
    /// @return The value of the setting.
    inline bool getSetting(const char* key) {
        CREATE_OPENHACK_DLL_METHOD("?getSetting@openhack@@YA_NPBD@Z", bool (*)(const char*), {
            return method(key);
        });
        return false;
    }

    /// @brief Contains methods for working with OpenHack's GUI.
    namespace ui {
        /// @brief Renders a label with the specified text.
        /// @param text The text to render.
        inline void label(char const* text) {
            CREATE_OPENHACK_DLL_METHOD("?label@ui@openhack@@YAXPBD@Z", void (*)(char const*), {
                return method(text);
            });
        }

        /// @brief Sets the width of the next element.
        /// @param width The width of the next element. (0.f to reset the width)
        inline void width(float width = 0.f) {
            CREATE_OPENHACK_DLL_METHOD("?width@ui@openhack@@YAXM@Z", void (*)(float), {
                return method(width);
            });
        }

        /// @brief Renders a textbox to input a float value.
        /// @param label The caption of the textbox.
        /// @param value Pointer to the float value that will be changed when the textbox is edited.
        /// @param min The minimum value that can be entered.
        /// @param max The maximum value that can be entered.
        /// @param format The format of the number (e.g. "%.3f").
        /// @return True if the value was changed, false otherwise.
        inline bool inputFloat(char const* label, float* value, float min = FLT_MIN, float max = FLT_MAX, char const* format = "%.3f") {
            CREATE_OPENHACK_DLL_METHOD("?inputFloat@ui@openhack@@YA_NPBDPAMMM0@Z", bool (*)(char const*, float*, float, float, char const*), {
                return method(label, value, min, max, format);
            });
            return false;
        }

        /// @brief Renders a textbox to input a text value.
        /// @param label The caption of the textbox.
        /// @param value Pointer to the text value that will be changed when the textbox is edited.
        /// @param maxLength The maximum length of the text.
        /// @param placeholder The placeholder text that will be displayed when the textbox is empty.
        /// @return True if the value was changed, false otherwise.
        inline bool inputText(char const* label, const std::string* value, int maxLength = 256, char const* placeholder = "") {
            CREATE_OPENHACK_DLL_METHOD("?inputText@ui@openhack@@YA_NPBDPAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H0@Z", bool (*)(char const*, const std::string*, int, char const*), {
                return method(label, value, maxLength, placeholder);
            });
            return false;
        }

        /// @brief Renders a button with the specified text.
        /// @param text The caption of the button.
        /// @param widthScale The width of the button relative to the remaining space in the window. (by default, the button will take up the entire width)
        /// @return True if the button was clicked, false otherwise.
        inline bool button(char const* text, float widthScale = 0.f) {
            CREATE_OPENHACK_DLL_METHOD("?button@ui@openhack@@YA_NPBDM@Z", bool (*)(char const*, float), {
                return method(text, widthScale);
            });
            return false;
        }

        /// @brief Renders a checkbox with the specified text.
        /// @param text The caption of the checkbox.
        /// @param value Pointer to the boolean value that will be changed when the checkbox is clicked.
        /// @return True if the checkbox was clicked, false otherwise.
        inline bool checkbox(char const* text, bool* value) {
            CREATE_OPENHACK_DLL_METHOD("?checkbox@ui@openhack@@YA_NPBDPA_N@Z", bool (*)(char const*, bool*), {
                return method(text, value);
            });
            return false;
        }

        /// @brief Add a tooltip to the last rendered element.
        /// @param text The text of the tooltip.
        inline void tooltip(char const* text) {
            CREATE_OPENHACK_DLL_METHOD("?tooltip@ui@openhack@@YAXPBD@Z", void (*)(char const*), {
                return method(text);
            });
        }

        /// @brief Adds a new window to the menu.
        /// @param title The title of the window.
        /// @param drawCallback The callback that will be called every frame to draw the window.
        /// @warning Call this method only once per window.
        inline void createWindow(char const* title, std::function<void()> const& drawCallback) {
            CREATE_OPENHACK_DLL_METHOD("?createWindow@ui@openhack@@YAXPBDABV?$function@$$A6AXXZ@std@@@Z", void (*)(char const*, std::function<void()> const&), {
                return method(title, drawCallback);
            });
        }
    }
}

#undef CREATE_OPENHACK_DLL_METHOD

#else
// Import the methods from the DLL.

namespace openhack {
    /// @brief Check whether any "cheats" are enabled (Those that trigger Cheat Indicator/Auto Safe Mode).
    OPENHACK_DLL bool isCheating();

    /// @brief Get the value of a setting stored in OpenHack's configuration.
    /// @param key The key of the setting.
    /// @return The value of the setting.
    OPENHACK_DLL bool getSetting(const char* key);

    /// @brief Contains methods for working with OpenHack's GUI.
    namespace ui {
        /// @brief Renders a label with the specified text.
        /// @param text The text to render.
        OPENHACK_DLL void label(char const* text);

        /// @brief Sets the width of the next element.
        /// @param width The width of the next element. (0.f to reset the width)
        OPENHACK_DLL void width(float width = 0.f);

        /// @brief Renders a textbox to input a float value.
        /// @param label The caption of the textbox.
        /// @param value Pointer to the float value that will be changed when the textbox is edited.
        /// @param min The minimum value that can be entered.
        /// @param max The maximum value that can be entered.
        /// @param format The format of the number (e.g. "%.3f").
        /// @return True if the value was changed, false otherwise.
        OPENHACK_DLL bool inputFloat(char const* label, float* value, float min = FLT_MIN, float max = FLT_MAX, char const* format = "%.3f");

        /// @brief Renders a textbox to input a text value.
        /// @param label The caption of the textbox.
        /// @param value Pointer to the text value that will be changed when the textbox is edited.
        /// @param maxLength The maximum length of the text.
        /// @param placeholder The placeholder text that will be displayed when the textbox is empty.
        /// @return True if the value was changed, false otherwise.
        OPENHACK_DLL bool inputText(char const* label, const std::string* value, int maxLength = 256, char const* placeholder = "");

        /// @brief Renders a button with the specified text.
        /// @param text The caption of the button.
        /// @param widthScale The width of the button relative to the remaining space in the window. (by default, the button will take up the entire width)
        /// @return True if the button was clicked, false otherwise.
        OPENHACK_DLL bool button(char const* text, float widthScale = 0.f);

        /// @brief Renders a checkbox with the specified text.
        /// @param text The caption of the checkbox.
        /// @param value Pointer to the boolean value that will be changed when the checkbox is clicked.
        /// @return True if the checkbox was clicked, false otherwise.
        OPENHACK_DLL bool checkbox(char const* text, bool* value);

        /// @brief Add a tooltip to the last rendered element.
        /// @param text The text of the tooltip.
        OPENHACK_DLL void tooltip(char const* text);

        /// @brief Adds a new window to the menu.
        /// @param title The title of the window.
        /// @param drawCallback The callback that will be called every frame to draw the window.
        /// @warning Call this method only once per window.
        OPENHACK_DLL void createWindow(char const* title, std::function<void()> const& drawCallback);
    }
}

#endif
#undef LOG_ERROR