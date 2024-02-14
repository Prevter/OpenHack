#include "menu.hpp"
#include "../openhack.hpp"
#include "../config.hpp"

namespace openhack::menu {
    bool isOpened = false;
    bool isInitialized = false;
    std::vector<gui::Window> windows;
    std::vector<gui::animation::MoveAction *> moveActions;

    ImVec2 randomWindowPosition(gui::Window &window) {
        auto start = window.getPosition();

        // Calculate target position randomly to be outside the screen
        // Choose random direction
        auto direction = utils::random(0, 7);
        auto screenSize = ImGui::GetIO().DisplaySize;
        ImVec2 target;

        switch (direction) {
            case 0: // Top
                target = ImVec2(start.x, -window.getSize().y);
                break;
            case 1: // Right
                target = ImVec2(screenSize.x, start.y);
                break;
            case 2: // Bottom
                target = ImVec2(start.x, screenSize.y);
                break;
            case 3: // Left
                target = ImVec2(-window.getSize().x, start.y);
                break;
            case 4: // Top-Left
                target = ImVec2(-window.getSize().x, -window.getSize().y);
                break;
            case 5: // Top-Right
                target = ImVec2(screenSize.x, -window.getSize().y);
                break;
            case 6: // Bottom-Right
                target = ImVec2(screenSize.x, screenSize.y);
                break;
            default: // Bottom-Left
                target = ImVec2(-window.getSize().x, screenSize.y);
                break;
        }

        return target;
    }

    void toggle() {
        isOpened = !isOpened;

        if (!isOpened) {
            // Save window positions
            config::set("windows", windows);
            config::save();
        }

        // Create animation
        double animationTime = config::get("menu.animationTime", 0.25);
        auto easingType = config::get("menu.easingType", gui::animation::Easing::Quadratic);
        auto easingMode = config::get("menu.easingMode", gui::animation::EasingMode::EaseInOut);
        auto easing = gui::animation::getEasingFunction(easingType, easingMode);

        if (isOpened) {
            for (auto &window: windows) {
                auto target = window.getPosition();
                moveActions.push_back(window.createMoveAction(target, animationTime, easing));
            }
        } else {
            for (auto &window: windows) {
                auto target = randomWindowPosition(window);
                moveActions.push_back(window.createMoveAction(target, animationTime, easing));
            }
        }
    }

    bool isOpen() { return isOpened; }

    void setVisibility(bool value) { isOpened = value; }

    bool isVisible() {
        // Whether the menu is visible or have running animations
        return isOpened || !moveActions.empty();
    }

    void init() {
        // Make sure to initialize ImGui
        gui::init();

        // Then initialize the windows
        if (isInitialized)
            return;

        addWindow("OpenHack", []() {
            gui::text("Version: " OPENHACK_VERSION);
            gui::text("Build: " __DATE__ " " __TIME__);
            gui::text("Game version: %s", utils::getGameVersion().c_str());

#ifdef OPENHACK_STANDALONE
            gui::checkbox("Check for updates", "menu.checkForUpdates");
#endif

            if (gui::button("Open GitHub page"))
                utils::openURL(OPENHACK_HOME_URL);

            if (gui::button("Join Discord server"))
                utils::openURL("https://discord.gg/QSd4jUyc45");
        });

        addWindow("Interface", []() {
            gui::width(70);
            gui::inputFloat("UI Scale", "menu.uiScale", 0.25f, 3.0f, "%.3f");
            gui::inputFloat("Border Size", "menu.borderSize", 0.0f, 10.0f, "%.3f");
            gui::inputFloat("Window Round", "menu.windowRounding", 0.0f, 10.0f, "%.3f");
            gui::inputFloat("Frame Round", "menu.frameRounding", 0.0f, 10.0f, "%.3f");
            gui::inputFloat("Window Snap", "menu.windowSnap", 0.0f, 10.0f, "%.3f");
            gui::width();

            gui::width(120);
            auto fonts = gui::getFonts();
            std::string fontsLine;
            for (auto &font: fonts)
                fontsLine += font.name + '\0';
            fontsLine += '\0';
            int32_t currentFont = gui::getFontIndex();
            if (gui::combo("Font", &currentFont, fontsLine.c_str())) {
                auto selected = fonts[currentFont].name;
                gui::setFont(selected);
            }
            if (gui::combo("Theme", "menu.theme", gui::THEME_NAMES, gui::THEME_COUNT)) {
                gui::setTheme(config::get<gui::Themes>("menu.theme"));
            }
            gui::width();

            gui::popupSettings("Colors", []() {
                gui::width(70);
                gui::colorEdit("Background", "menu.color.background");
                gui::colorEdit("Accent", "menu.color.accent");
                gui::colorEdit("Primary", "menu.color.primary");
                gui::colorEdit("Secondary", "menu.color.secondary");
                gui::colorEdit("Border", "menu.color.border");
                gui::colorEdit("Hovered", "menu.color.hovered");
                gui::colorEdit("Clicked", "menu.color.clicked");
                gui::colorEdit("Text", "menu.color.text");
                gui::colorEdit("Text Disabled", "menu.color.textDisabled");
                gui::width();

                if (gui::button("Reset")) {
                    gui::loadPalette();
                }
            });

            // gui::toggleSetting(
            //     "Rainbow",
            //     "menu.rainbow",
            //     []()
            //     {
            //         gui::inputFloat("Speed", "menu.rainbowSpeed", 0.0f, 10.0f, "%.3f");
            //     });

            gui::width(70);
            gui::inputFloat("Animation Time", "menu.animationTime", 0.0f, 10.0f, "%.3f");
            gui::width();
            gui::width(120);
            gui::combo("Easing", "menu.easingType", gui::animation::EASING_NAMES, gui::animation::EASING_COUNT);
            gui::combo("Mode", "menu.easingMode", gui::animation::EASING_MODE_NAMES, 3);
            gui::combo("Blur", "menu.blur", "Off\0Windows\0Screen\0\0");
            gui::width();


            if (gui::button("Reorder Windows"))
                stackWindows();

            gui::checkbox("Lock Windows", "menu.stackWindows");
        });

        // Make all windows start outside the screen
        for (auto &window: windows) {
            auto target = randomWindowPosition(window);
            auto *action = window.createMoveAction(target, 0, gui::animation::easing::linear);
            action->update(0);
            delete action;
        }

        // Restore window positions from config
//        if (config::has("windows"))
//        {
//            auto loaded = config::get<std::vector<gui::Window>>("windows");
//            for (auto &window : windows)
//            {
//                auto it = std::find_if(loaded.begin(), loaded.end(), [&window](const gui::Window &w)
//                                       { return w.getTitle() == window.getTitle(); });
//                if (it != loaded.end())
//                {
//                    window.setOpen(it->isOpen());
//                    window.setPosition(it->getPosition());
//                }
//            }
//        }

        isInitialized = true;
    }

    void draw() {
        utils::resetKeyStates();

        if (utils::isKeyPressed(config::get<std::string>("menu.toggleKey", "Tab")))
            toggle();

        // Run move actions
        for (auto &action: moveActions) {
            action->update(utils::getDeltaTime());
        }

        // Remove finished actions
        moveActions.erase(
                std::remove_if(
                        moveActions.begin(), moveActions.end(),
                        [](gui::animation::MoveAction *action) {
                            if (action->isFinished()) {
                                delete action;
                                return true;
                            }
                            return false;
                        }),
                moveActions.end());

        if (!isVisible())
            return;

        // Update theme
        gui::setStyles();

        // Draw all windows
        for (auto &window: windows) {
            window.draw();
        }

        if (config::get<bool>("menu.stackWindows"))
            stackWindows();
    }

    void addWindow(const std::string &title, const std::function<void()> &onDraw) {
        windows.emplace_back(title, onDraw);
    }

    std::map<gui::Window *, ImVec2> getStackedPositions() {
        const std::string builtInWindows[] = {"OpenHack", "Interface", "Keybinds"};
        const size_t builtInCount = sizeof(builtInWindows) / sizeof(builtInWindows[0]);
        auto snap = config::get<float>("menu.windowSnap");

        ImVec2 screenSize = ImGui::GetIO().DisplaySize;

        float windowWidth = gui::Window::MIN_SIZE.x;
        size_t columns = (screenSize.x - snap) / (windowWidth + snap);

        std::map<gui::Window *, ImVec2> positions;

        // Built-ins go into first column
        float x = snap;
        float y = snap;
        for (auto &title: builtInWindows) {
            auto it = std::find_if(windows.begin(), windows.end(),
                                   [&title](const gui::Window &window) { return window.getTitle() == title; });
            if (it != windows.end()) {
                positions[&(*it)] = ImVec2(x, y);
                y += it->getSize().y + snap;
            }
        }

        return positions;
    }

    void stackWindows() {
        auto animationTime = config::get<float>("menu.animationTime");
        auto easingType = config::get<gui::animation::Easing>("menu.easingType");
        auto easingMode = config::get<gui::animation::EasingMode>("menu.easingMode");
        auto easing = gui::animation::getEasingFunction(easingType, easingMode);

        auto positions = getStackedPositions();
        for (auto &pair: positions) {
            // Check if the window is already in the correct position
            if (pair.first->getPosition().x == pair.second.x && pair.first->getPosition().y == pair.second.y)
                continue;

            auto *action = pair.first->createMoveAction(pair.second, animationTime, easing, true);
            moveActions.push_back(action);
        }
    }
}