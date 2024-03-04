#include "config.hpp"
#include "openhack.hpp"

#include <fstream>

namespace openhack::config {
    void setDefaults() {
        setIfEmpty("menu.animationTime", 0.35);
        setIfEmpty("menu.easingType", gui::animation::Easing::Cubic);
        setIfEmpty("menu.easingMode", gui::animation::EasingMode::EaseInOut);
        setIfEmpty("menu.uiScale", 1.f);
        setIfEmpty("menu.borderSize", 0.f);
        setIfEmpty("menu.windowRounding", 4.f);
        setIfEmpty("menu.frameRounding", 4.f);
        setIfEmpty("menu.windowSnap", 3.f);
        setIfEmpty("menu.stackWindows", true);
        setIfEmpty("menu.theme", gui::Themes::MegaHack);
        setIfEmpty("menu.toggleKey", "Tab");
        setIfEmpty("menu.font", "Poppins");
        setIfEmpty("menu.fontSize", 20.f);
        setIfEmpty("menu.rainbow.enabled", false);
        setIfEmpty("menu.rainbow.speed", 0.15f);
        setIfEmpty("menu.rainbow.saturation", 65.0f);
        setIfEmpty("menu.rainbow.value", 65.0f);
        setIfEmpty("menu.checkForUpdates", true);
        setIfEmpty("keybinds.ingame", false);
    }

    void load() {
        auto path = utils::getModSaveDirectory() / "config.json";
        std::ifstream file(path);

        if (!file.is_open()) {
            L_WARN("Config file does not exist, creating a new one");
            save();
            return;
        }

        file >> storage;
        file.close();
    }

    void save() {
        auto path = utils::getModSaveDirectory() / "config.json";
        std::ofstream file(path);
        file << storage.dump(4);
        file.close();
    }
}

void to_json(nlohmann::json &j, const ImVec2 &v) {
    j = nlohmann::json{{"x", v.x},
                       {"y", v.y}};
}

void from_json(const nlohmann::json &j, ImVec2 &v) {
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
}

namespace openhack::gui::animation {
    void to_json(nlohmann::json &j, const Easing &e) {
        j = static_cast<int>(e);
    }

    void from_json(const nlohmann::json &j, Easing &e) {
        e = static_cast<Easing>(j.get<int>());
    }

    void to_json(nlohmann::json &j, const EasingMode &e) {
        j = static_cast<int>(e);
    }

    void from_json(const nlohmann::json &j, EasingMode &e) {
        e = static_cast<EasingMode>(j.get<int>());
    }
}

namespace openhack::gui {
    void to_json(nlohmann::json &j, const Color &e) {
        auto str = e.toString();
        j = str;
    }

    void from_json(const nlohmann::json &j, Color &e) {
        e = Color::fromString(j.get<std::string>().c_str());
    }

    void to_json(nlohmann::json &j, const Themes &e) {
        j = static_cast<int>(e);
    }

    void from_json(const nlohmann::json &j, Themes &e) {
        e = static_cast<Themes>(j.get<int>());
    }

    void to_json(nlohmann::json &j, const Window &e) {
        j = nlohmann::json{{"pos",   e.getPosition()},
                           {"open",  e.isOpen()},
                           {"title", e.getTitle()}};
    }

    void from_json(const nlohmann::json &j, Window &e) {
        e.setPosition(j.at("pos").get<ImVec2>());
        e.setOpen(j.at("open").get<bool>());
        e.setTitle(j.at("title").get<std::string>());
    }

    void to_json(nlohmann::json &j, const std::vector<Window> &e) {
        j = nlohmann::json::array();
        for (const auto &window: e) {
            j.push_back(window);
        }
    }

    void from_json(const nlohmann::json &j, std::vector<Window> &e) {
        e.clear();
        for (const auto &windowJ: j) {
            e.push_back(windowJ.get<Window>());
        }
    }
}

namespace openhack::menu::blur {
    void to_json(nlohmann::json &j, const State &e) {
        j = static_cast<int>(e);
    }

    void from_json(const nlohmann::json &j, State &e) {
        e = static_cast<State>(j.get<int>());
    }
}

namespace openhack::menu::keybinds {
    void to_json(nlohmann::json &j, const Keybind &e) {
        j = nlohmann::json{{"name",    e.name},
                           {"id",      e.id},
                           {"keycode", e.keycode}};
    }

    void from_json(const nlohmann::json &j, Keybind &e) {
        e.name = j.at("name").get<std::string>();
        e.id = j.at("id").get<std::string>();
        e.keycode = j.at("keycode").get<uint32_t>();
    }

    void to_json(nlohmann::json &j, const std::vector<Keybind> &e) {
        j = nlohmann::json::array();
        for (const auto &keybind: e) {
            j.push_back(keybind);
        }
    }

    void from_json(const nlohmann::json &j, std::vector<Keybind> &e) {
        e.clear();
        for (const auto &keybindJ: j) {
            std::string name = keybindJ.at("name").get<std::string>();
            std::string id = keybindJ.at("id").get<std::string>();
            uint32_t keycode = keybindJ.at("keycode").get<uint32_t>();
            e.emplace_back(name, id, keycode);
        }
    }
}