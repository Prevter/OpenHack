#pragma once

#include <string>
#include <functional>

#if OPENHACK_EXPORT
    #define OPENHACK_DLL __declspec(dllexport)
#else
    #define OPENHACK_DLL __declspec(dllimport)
#endif

#ifdef OPENHACK_OPTIONAL
// Call the methods directly from the DLL.
#define OPENHACK_DLL_NAME "prevter.openhack.dll"

#define CREATE_OPENHACK_DLL_METHOD(signature, callingConvention, methodBody)  \
    auto library = GetModuleHandleA(OPENHACK_DLL_NAME);           \
    if (library) {                                                \
        auto method = reinterpret_cast<callingConvention>(GetProcAddress(library, signature)); \
        methodBody                                                \
    }                                                             \
    geode::log::error("OpenHack is not loaded into the game!")    \

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

    /// @brief Contains methods for working with OpenHack's GUI.
    namespace ui {
        /// @brief Renders a label with the specified text.
        /// @param text The text to render.
        inline void label(char const* text) {
            CREATE_OPENHACK_DLL_METHOD("?label@ui@openhack@@YAXPBD@Z", void (*)(char const*), {
                return method(text);
            });
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

    /// @brief Contains methods for working with OpenHack's GUI.
    namespace ui {
        /// @brief Renders a label with the specified text.
        /// @param text The text to render.
        OPENHACK_DLL void label(char const* text);

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

        /// @brief Adds a new window to the menu.
        /// @param title The title of the window.
        /// @param drawCallback The callback that will be called every frame to draw the window.
        /// @warning Call this method only once per window.
        OPENHACK_DLL void createWindow(char const* title, std::function<void()> const& drawCallback);
    }
}

#endif

