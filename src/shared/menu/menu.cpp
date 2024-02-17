#include "menu.hpp"
#include "../openhack.hpp"
#include "../config.hpp"
#include "../hacks/hacks.hpp"
// #include "blur.hpp"

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

        // If the window is collapsed, multiply the target position by 1.5
        if (!window.isOpen()) {
            target.x *= 1.5;
            target.y *= 1.5;
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

#ifdef PLATFORM_WINDOWS

    void patchGame() {
        bool success = win32::four_gb::patch();
        L_INFO("Patched the game to use 4GB of memory: {}", success);
        MessageBox(nullptr, success ? "Patched the game to use 4GB of memory. Please restart the game."
                                    : "Failed to patch the game. Could not write to the file.",
                   "4GB Patch", (success ? MB_ICONINFORMATION : MB_ICONERROR) | MB_OK);

        if (success) {
            // Close the game
            std::exit(0);
        }
    }

#endif

    void init() {
        // Make sure to initialize ImGui
        gui::init();
        // blur::compileShader();

        // Then initialize the windows
        if (isInitialized)
            return;

#ifdef PLATFORM_WINDOWS
        if (!win32::four_gb::isPatched()) {
            L_WARN("The game is not patched to use 4GB of memory");
        }
#endif

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

#ifdef PLATFORM_WINDOWS
            if (!win32::four_gb::isPatched() && gui::button("Apply 4GB patch")) {
                patchGame();
            }
            gui::tooltip("Highly recommended to install.\n"
                         "Allows the game to use more memory, which resolves some crashes.");

            if (gui::button("Inject DLL")) {
                win32::promptDllInjection();
            }
#ifdef OPENHACK_GEODE
            gui::tooltip("DLL injection is not recommended when using Geode.");
#endif
#endif
        });

        addWindow("Interface", []() {
            gui::width(70);
            gui::inputFloat("UI Scale", "menu.uiScale", 0.25f, 3.0f, "%.3f");
            gui::inputFloat("Border Size", "menu.borderSize", 0.0f, 10.0f, "%.3f");
            gui::inputFloat("Window Rounding", "menu.windowRounding", 0.0f, 10.0f, "%.3f");
            gui::inputFloat("Frame Rounding", "menu.frameRounding", 0.0f, 10.0f, "%.3f");
            gui::inputFloat("Window Margin", "menu.windowSnap", 0.0f, 10.0f, "%.3f");
            gui::width();

            gui::width(120);
            auto fonts = gui::getFonts();
            std::string fontsLine; // Create null-delimited string with all font names
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
                gui::loadPalette();
            }
            gui::width();

            gui::popupSettings("Colors", []() {
                gui::width(120);
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
            gui::width(110);
            gui::combo("Easing Type", "menu.easingType", gui::animation::EASING_NAMES, gui::animation::EASING_COUNT);
            gui::combo("Easing Mode", "menu.easingMode", gui::animation::EASING_MODE_NAMES, 3);
            // TODO: Implement blur properly
            // if (gui::combo("Blur", "menu.blur", "Off\0Windows\0Screen\0\0")) {
            //     blur::setState(config::get<blur::State>("menu.blur"));
            // }
            gui::width();

            if (gui::button("Reorder Windows"))
                stackWindows();

            gui::checkbox("Lock Windows", "menu.stackWindows");
        });

        auto hacks = hacks::getWindows();
        for (auto &hack: hacks) {
            windows.push_back(hack);
        }

        // Make all windows start outside the screen
        for (auto &window: windows) {
            auto target = randomWindowPosition(window);
            window.setDrawPosition(target);
        }

        // Restore window positions from config
        if (config::has("windows")) {
            auto loaded = config::get<std::vector<gui::Window>>("windows");
            for (auto &window: windows) {
                auto it = std::find_if(loaded.begin(), loaded.end(),
                                       [&window](const gui::Window &w) { return w.getTitle() == window.getTitle(); });
                if (it != loaded.end()) {
                    window.setOpen(it->isOpen());
                    window.setPosition(it->getPosition());
                }
            }
        }

        isInitialized = true;
    }

    void draw() {
        utils::resetKeyStates();

        if (utils::isKeyPressed(config::get<std::string>("menu.toggleKey", "Tab")))
            toggle();

        keybinds::update();

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

        // Show mouse cursor
        auto *glView = gd::cocos2d::CCEGLView::sharedOpenGLView();
        gd::cocos2d::CCEGLView::showCursor(glView, true);

        // Update theme
        gui::setStyles();

        // Draw all windows
        for (auto &window: windows) {
            window.draw();
        }

        if (config::get<bool>("menu.stackWindows") && moveActions.empty())
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

        const auto scale = config::get<float>("menu.uiScale");
        float windowWidth = gui::Window::MIN_SIZE.x * scale;
        auto columns = (size_t) ((screenSize.x - snap) / (windowWidth + snap));

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

        if (columns == 0)
            return positions;

        // Rest are stacked to take as little space as possible
        std::vector<float> heights(columns - 1, snap);
        for (auto &window: windows) {
            // Skip built-in windows
            if (std::find(builtInWindows, builtInWindows + builtInCount, window.getTitle()) !=
                builtInWindows + builtInCount)
                continue;

            // Find the column with the smallest height
            auto min = std::min_element(heights.begin(), heights.end());
            auto index = std::distance(heights.begin(), min);

            // Set the position
            positions[&window] = ImVec2((float) (index + 1) * (windowWidth + snap) + snap, *min);
            *min += window.getSize().y + snap;

            // Update the height
            heights[index] = *min;
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