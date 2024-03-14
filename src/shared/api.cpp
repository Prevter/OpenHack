#include "openhack.hpp"
#include "../../api/openhack.hpp"

namespace openhack {
    OPENHACK_DLL bool isCheating() {
        return config::getGlobal<bool>("isCheating", false);
    }

    namespace ui {
        OPENHACK_DLL void label(char const* text) {
            gui::text("%s", text);
        }

        OPENHACK_DLL bool button(char const* text, float widthScale) {
            return gui::button(text, {widthScale, 0});
        }

        OPENHACK_DLL bool checkbox(char const* text, bool* value) {
            return gui::checkbox(text, value);
        }

        OPENHACK_DLL void createWindow(char const* title, std::function<void()> const& drawCallback) {
            menu::addWindow(title, drawCallback);
        }
    }
}