#include "menu.hpp"
#include "../openhack.hpp"
#include "../config.hpp"
#include "../hacks/hacks.hpp"
// #include "blur.hpp"

#ifdef OPENHACK_STANDALONE

#include <imgui_markdown.h>
#include "../../standalone/updater/updater.hpp"

float *downloadProgress = nullptr;

void markdownOpenLink(ImGui::MarkdownLinkCallbackData data) { openhack::utils::openURL(data.link); }

#endif

namespace openhack::menu {
    bool isOpened = false;
    bool isInitialized = false;
    bool isAnimating = false;
    std::vector<gui::Window> windows;
    std::vector<gui::animation::MoveAction *> moveActions;

    ImVec2 randomWindowPosition(gui::Window &window) {
        // Calculate target position randomly to be outside the screen
        auto screenSize = ImGui::GetIO().DisplaySize;
        auto windowSize = window.getSize();
        ImVec2 target;

        // Pick a random side of the screen
        auto side = utils::random(3);
        switch (side) {
            case 0:
                target = ImVec2(utils::random(screenSize.x - windowSize.x), -windowSize.y);
                break;
            case 1:
                target = ImVec2(utils::random(screenSize.x - windowSize.x), screenSize.y);
                break;
            case 2:
                target = ImVec2(-windowSize.x, utils::random(screenSize.y - windowSize.y));
                break;
            case 3:
            default:
                target = ImVec2(screenSize.x, utils::random(screenSize.y - windowSize.y));
                break;
        }

        return target;
    }

    inline void updateCursorState() {
        bool canShowInLevel = true;
        if (auto *playLayer = gd::PlayLayer::get()) {
            canShowInLevel = playLayer->m_hasCompletedLevel() ||
                             playLayer->m_isPaused() ||
                             gd::GameManager::sharedState()->getGameVariable("0024");

            // "Click Teleport" enables cursor, so we need to check if it's enabled
            if (config::get<bool>("hack.click_tp.enabled", false)) return;
        }
        gd::cocos2d::CCEGLView::sharedOpenGLView()->showCursor(isOpened || canShowInLevel);
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

        // Update cursor state
        updateCursorState();

        isAnimating = true;
    }

    bool isOpen() { return isOpened; }

    void setVisibility(bool value) { isOpened = value; }

    bool isVisible() {
        // Whether the menu is visible or have running animations
        return isOpened || !moveActions.empty();
    }

    void createOpenURLPopup(const std::string &url) {
        gui::Modal::create("Open URL", [url](gui::Modal *popup) {
            ImGui::TextWrapped("You are about to open\n%s in your browser.", url.c_str());
            ImGui::TextWrapped("Do you want to continue?");
            if (gui::button("Yes", {0.5f, 0.f})) {
                utils::openURL(url.c_str());
                popup->close();
            }
            ImGui::SameLine(0, 2);
            if (gui::button("No")) {
                popup->close();
            }
        });
    }

    void init() {
        // Make sure to initialize ImGui
        gui::init();

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
                createOpenURLPopup(OPENHACK_HOME_URL);

            if (gui::button("Join Discord server"))
                createOpenURLPopup("https://discord.gg/QSd4jUyc45");

            auto searchValue = config::getGlobal<std::string>("searchValue", "");
            gui::widthF(1.0);
            gui::inputText("##Search", &searchValue, 64, "Search");
            gui::width();
            config::setGlobal("searchValue", searchValue);
        });

        addWindow("Interface", []() {
            gui::width(120);
            if (gui::combo("Theme", "menu.theme", gui::THEME_NAMES, gui::THEME_COUNT)) {
                gui::setTheme(config::get<gui::Themes>("menu.theme"));
                gui::loadPalette();
            }
            gui::width();

            gui::popupSettings("Sizes", []() {
                gui::width(70);
                gui::inputFloat("UI Scale", "menu.uiScale", 0.5f, 3.0f, "%.3f");
                gui::inputFloat("Border Size", "menu.borderSize", 0.0f, 10.0f, "%.3f");
                gui::inputFloat("Window Rounding", "menu.windowRounding", 0.0f, 10.0f, "%.3f");
                gui::inputFloat("Frame Rounding", "menu.frameRounding", 0.0f, 10.0f, "%.3f");
                gui::inputFloat("Window Margin", "menu.windowSnap", 0.0f, 10.0f, "%.3f");
                gui::width();
            });

            gui::popupSettings("Fonts", []() {
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
                gui::inputFloat("Font Size", "menu.fontSize", 8.0f, 32.0f, "%.1f px");
                gui::width();

                if (gui::button("Reload Fonts")) {
                    config::setGlobal("resetNextFrame", true);
                    return;
                }
                gui::tooltip("Reloads the UI to apply changes which require a restart");
            });

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

            gui::popupSettings("Animations", []() {
                gui::width(70);
                gui::inputFloat("Animation Time", "menu.animationTime", 0.0f, 10.0f, "%.3f");
                gui::width();
                gui::width(110);
                gui::combo("Easing Type", "menu.easingType", gui::animation::EASING_NAMES,
                           gui::animation::EASING_COUNT);
                gui::combo("Easing Mode", "menu.easingMode", gui::animation::EASING_MODE_NAMES, 3);
                gui::width();
                gui::checkbox("Animate Opacity", "menu.animateOpacity");
            });

            // TODO: Implement blur properly
            // if (gui::combo("Blur", "menu.blur", "Off\0Windows\0Screen\0\0")) {
            //     blur::setState(config::get<blur::State>("menu.blur"));
            // }

            gui::callback([]() {
                gui::tooltip("Makes the title bar change colors.");
            });
            gui::toggleSetting(
                    "Rainbow Menu",
                    "menu.rainbow.enabled",
                    []() {
                        gui::width(110);
                        gui::inputFloat("Speed", "menu.rainbow.speed");
                        gui::inputFloat("Saturation", "menu.rainbow.saturation", 0.0f, 100.0f);
                        gui::inputFloat("Value", "menu.rainbow.value", 0.0f, 100.0f);
                        gui::width();
                    });

            if (gui::button("Reorder Windows"))
                stackWindows();
            gui::tooltip("Reorganizes the windows to take as little space as possible");

            gui::checkbox("Lock Windows", "menu.stackWindows");

        });

        // Get windows for hacks
        auto hacks = hacks::getWindows();
        for (auto &hack: hacks) {
            windows.push_back(hack);
        }

        // Call late init for embedded hacks
        for (auto &hack: hacks::getEmbeddedHacks()) {
            hack->onLateInit();
        }

        // Restore window positions from config
        if (config::has("windows")) {
            auto loaded = config::get<std::vector<gui::Window>>("windows");
            for (auto &window: windows) {
                auto it = std::find_if(loaded.begin(), loaded.end(),
                                       [&window](const gui::Window &w) { return w.getTitle() == window.getTitle(); });
                if (it != loaded.end()) {
                    auto opened = it->isOpen();
                    auto position = it->getPosition();
                    window.setOpen(opened);
                    window.setPosition(position);
                    window.setDrawPosition(position);
                }
            }
        }

        // Sort windows by title
        std::sort(windows.begin(), windows.end(), [](const gui::Window &a, const gui::Window &b) {
            return a.getTitle() < b.getTitle();
        });

        isInitialized = true;

    }

    uint8_t firstRunState = 0;

    void updateColors() {
        // Rainbow menu
        bool rainbowEnabled = config::get<bool>("menu.rainbow.enabled");
        gui::Color accentOrig;
        if (rainbowEnabled) {
            accentOrig = config::get<gui::Color>("menu.color.accent");

            // Calculate new colors
            auto speed = config::get<float>("menu.rainbow.speed");
            auto saturation = config::get<float>("menu.rainbow.saturation");
            auto value = config::get<float>("menu.rainbow.value");

            float r, g, b;
            ImGui::ColorConvertHSVtoRGB(
                    (float) ImGui::GetTime() * speed,
                    saturation / 100.0f,
                    value / 100.0f,
                    r, g, b);

            gui::Color primary = {r, g, b, accentOrig.a};
            config::set("menu.color.accent", primary);
        }

        // Update theme
        gui::setStyles();

        // Revert rainbow menu colors
        if (rainbowEnabled) {
            config::set("menu.color.accent", accentOrig);
        }
    }

    void draw() {
        // Calculate relative UI scale against 1080p
        auto resW = ImGui::GetIO().DisplaySize.x;
        auto ratio = resW / 1920.0f;
        auto currentScale = config::get<float>("menu.uiScale");
        config::setGlobal("UIScale", ratio * currentScale);

        // Get the window sizes for the first run
        switch (firstRunState) {
            case 0:
                gui::setStyles();
                ImGui::GetStyle().Alpha = 0.0f;
                for (auto &window: windows) {
                    window.draw();
                }
                firstRunState = 1;
                break;
            case 1:
                // Make all windows start outside the screen
                for (auto &window: windows) {
                    // Render the window once to get the size
                    window.draw();
                    auto target = randomWindowPosition(window);
                    window.setDrawPosition(target);
                }
                ImGui::GetStyle().Alpha = 1.0f;
                firstRunState = 2;
                return;
            default:
                break;
        }


#ifdef OPENHACK_STANDALONE
        // Check for updates
        if (config::getGlobal<bool>("update.available", false)) {
            gui::Modal::create("Update Available", [](gui::Modal* modal){
                gui::text("A new version of OpenHack is available!");
                gui::text("Current version: " OPENHACK_VERSION);
                gui::text("Latest version: %s", config::getGlobal<std::string>("update.version").c_str());

                std::string content = "# " + config::getGlobal<std::string>("update.title") + "\n" +
                                      config::getGlobal<std::string>("update.changelog");

                ImGui::MarkdownConfig mdConfig;
                mdConfig.linkCallback = markdownOpenLink;
                mdConfig.tooltipCallback = nullptr;
                mdConfig.imageCallback = nullptr;
                mdConfig.userData = nullptr;
                auto font = gui::getFont();
                mdConfig.headingFormats[0] = {font.title, true};
                mdConfig.headingFormats[1] = {font.title, false};
                mdConfig.headingFormats[2] = {font.normal, false};

                ImGui::Markdown(content.c_str(), content.length(), mdConfig);

                if (downloadProgress) {
                    if (*downloadProgress == 1.0f)
                        gui::text("Installing update...");
                    else
                        gui::progressBar(*downloadProgress);
                } else {
                    if (gui::button("Download", ImVec2(0.5, 0))) {
                        downloadProgress = new float(0.0f);
                        updater::install(config::getGlobal<std::string>("update.downloadUrl"), downloadProgress);
                    }
                    ImGui::SameLine(0, 0);
                    if (gui::button("Close")) {
                        modal->close();
                    }
                }
            });
            config::setGlobal("update.available", false);
        }
#endif

        utils::resetKeyStates();

        if (utils::isKeyPressed(config::get<std::string>("menu.toggleKey", "Tab")))
            toggle();

        keybinds::update();

        // Run move actions
        for (auto &action: moveActions) {
            action->update(utils::getDeltaTime());
        }

        // Change opacity of the menu to the latest action progress
        if (isAnimating && !moveActions.empty() &&
            config::get<float>("menu.animationTime") > 0 &&
            config::get<bool>("menu.animateOpacity", false)) {
            auto lastAction = moveActions.back();
            auto progress = std::clamp(lastAction->getProgress(), 0.0, 1.0);
            if (!isOpened) progress = 1.0 - progress;
            ImGui::GetStyle().Alpha = static_cast<float>(progress);
        }

        // Update embedded hacks
        for (auto &hack: hacks::getEmbeddedHacks()) {
            hack->update();
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

        // Reset animation flag if there are no more actions
        if (moveActions.empty()) {
            isAnimating = false;
        }

        if (!isVisible()) {
            if (!gui::getPopups().empty()) {
                updateColors();
                gui::drawPopups();
            }
            return;
        }

        // Show mouse cursor if the menu is open
        updateCursorState();

        // Update colors
        updateColors();

        // Draw all windows
        for (auto &window: windows) {
            window.draw();
        }

        gui::drawPopups();

        // Auto reset window positions
        auto isDragging = config::getGlobal("draggingWindow", false);
        auto stackEnabled = config::get<bool>("menu.stackWindows");
        if (moveActions.empty() && !isDragging && stackEnabled)
            stackWindows();

        // Reset dragging state
        config::setGlobal("draggingWindow", false);

        // Reset UI if requested
        if (config::getGlobal<bool>("resetNextFrame", false)) {
            config::setGlobal("resetNextFrame", false);
            utils::resetUI();
        }
    }

    void addWindow(const std::string &title, const std::function<void()> &onDraw) {
        windows.emplace_back(title, onDraw);
    }

    std::map<gui::Window *, ImVec2> getStackedPositions() {
        const std::string builtInWindows[] = {"OpenHack", "Interface", "Keybinds"};
        const size_t builtInCount = sizeof(builtInWindows) / sizeof(builtInWindows[0]);
        auto snap = config::get<float>("menu.windowSnap");

        ImVec2 screenSize = ImGui::GetIO().DisplaySize;

        const auto scale = config::getGlobal<float>("UIScale");
        float windowWidth = gui::Window::MIN_SIZE.x * scale;
        auto columns = (int) ((screenSize.x - snap) / (windowWidth + snap));

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

        if (columns <= 0)
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