#include "openhack.hpp"
#include "../../api/openhack.hpp"

namespace openhack {
    OPENHACK_DLL bool isCheating() {
        return config::getGlobal<bool>("isCheating", false);
    }

    OPENHACK_DLL bool getSetting(const char* key) {
        if (config::has(key) && config::is<bool>(key)) {
            return config::get<bool>(key);
        }
        return false;
    }

    namespace ui {
        OPENHACK_DLL void label(char const* text) {
            gui::text("%s", text);
        }

        OPENHACK_DLL void width(float width) {
            gui::width(width);
        }

        OPENHACK_DLL bool inputFloat(char const* label, float* value, float min, float max, char const* format) {
            return gui::inputFloat(label, value, min, max, format);
        }

        OPENHACK_DLL bool inputText(char const* label, std::string* value, int maxLength, char const* placeholder) {
            return gui::inputText(label, value, maxLength, placeholder);
        }

        OPENHACK_DLL bool button(char const* text, float widthScale) {
            return gui::button(text, {widthScale, 0});
        }

        OPENHACK_DLL bool checkbox(char const* text, bool* value) {
            return gui::checkbox(text, value);
        }

        OPENHACK_DLL void tooltip(char const* text) {
            gui::tooltip(text);
        }

        OPENHACK_DLL void createWindow(char const* title, std::function<void()> const& drawCallback) {
            menu::addWindow(title, drawCallback);
        }
    }
}