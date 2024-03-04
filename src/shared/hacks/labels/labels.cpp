#include "labels.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    void LabelsContainer::addLabel(Label *label, const std::function<void(Label *)> &update) {
        m_labels.emplace_back(label, update);
    }

    void LabelsContainer::removeLabel(Label *label) {
        m_labels.erase(std::remove_if(m_labels.begin(), m_labels.end(), [label](const auto &pair) {
            return pair.first == label;
        }), m_labels.end());
    }

    void LabelsContainer::recalculatePositions() {
        if (gd::PlayLayer::get() == nullptr) return;

        auto *playLayer = gd::PlayLayer::get();
        auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        auto padding = config::get<float>("hack.labels.padding");
        ImVec2 anchor = {0, 0};
        ImVec2 offset = {0, 0};
        float verticalSign = 1.0f;

        switch (m_position) {
            case ContainerPosition::TopLeft:
                anchor = {0, 1};
                offset = {padding, winSize.height - padding};
                break;
            case ContainerPosition::Top:
                anchor = {0.5, 1};
                offset = {winSize.width / 2, winSize.height - padding};
                break;
            case ContainerPosition::TopRight:
                anchor = {1, 1};
                offset = {winSize.width - padding, winSize.height - padding};
                break;
            case ContainerPosition::BottomLeft:
                anchor = {0, 0};
                offset = {padding, padding};
                verticalSign = -1.0f;
                break;
            case ContainerPosition::Bottom:
                anchor = {0.5, 0};
                offset = {winSize.width / 2, padding};
                verticalSign = -1.0f;
                break;
            case ContainerPosition::BottomRight:
                anchor = {1, 0};
                offset = {winSize.width - padding, padding};
                verticalSign = -1.0f;
                break;
        }

        for (auto &label: m_labels) {
            if (!label.first->isVisible()) continue;

            label.first->setAnchor(anchor);
            label.first->setPosition(offset);

            offset.y -= label.first->getHeight() * verticalSign * label.first->getScale();
        }
    }

    void LabelsContainer::recreateLabels() {
        auto *playLayer = gd::PlayLayer::get();
        if (playLayer == nullptr) return;

        for (auto &label: m_labels) {
            label.first->addToLayer(playLayer);
        }
    }

    void LabelsContainer::clear() {
        // Delete all labels
        for (auto &label: m_labels) {
            delete label.first;
        }
        m_labels.clear();
    }

    void LabelsContainer::deleteFromScene() {
        auto *playLayer = gd::PlayLayer::get();
        if (playLayer == nullptr) return;

        for (auto &label: m_labels) {
            label.first->remove(playLayer);
        }

        clear();
    }

    void LabelsContainer::update() {
        if (gd::PlayLayer::get() == nullptr) return;

        recalculatePositions();

        if (!m_visible) {
            for (auto &label: m_labels) {
                label.first->setVisible(false);
            }
            return;
        }

        for (auto &label: m_labels) {
            label.second(label.first);
        }
    }

    LabelsContainer CONTAINERS[6] = {
            LabelsContainer(ContainerPosition::TopLeft),
            LabelsContainer(ContainerPosition::Top),
            LabelsContainer(ContainerPosition::TopRight),
            LabelsContainer(ContainerPosition::BottomLeft),
            LabelsContainer(ContainerPosition::Bottom),
            LabelsContainer(ContainerPosition::BottomRight)
    };

    void respawnLabels() {
        for (auto &container: CONTAINERS) {
            container.deleteFromScene();
        }
        Labels::playLayerLateInit();
    }

    static std::vector<LabelConfig> s_labels;

    const LabelConfig PRESETS[]{
            {
                    .caption = "FPS",
                    .text = "FPS: {fps}",
            },
            {
                    .caption = "CPS",
                    .text = "{cps} CPS",
            },
            {
                    .caption = "Noclip Accuracy",
                    .text = "Accuracy: {noclip_acc}%",
            },
            {
                    .caption = "Attempts",
                    .text = "Attempt {attempts}",
            },
            {
                    .caption = "Progress",
                    .text = "Progress: {progress}%",
            },
            {
                    .caption = "Clock",
                    .text = "{clock}",
            },
            {
                    .caption = "Best",
                    .text = "Best: {best}%",
            },
            {
                    .caption = "Empty",
                    .text = "",
            }
    };

    const char *const PRESET_NAMES[]{
            "FPS",
            "CPS",
            "Noclip Accuracy",
            "Attempts",
            "Progress",
            "Clock",
            "Best",
            "Empty"
    };

    const size_t PRESET_COUNT = sizeof(PRESETS) / sizeof(LabelConfig);
    static int32_t s_currentPreset = 0;

    void Labels::onInit() {
        // Set the default value
        config::setIfEmpty("hack.labels.enabled", true);
        config::setIfEmpty("hack.labels.padding", 5.0f);
        config::setIfEmpty("hack.cheat-indicator.enabled", true);
        config::setIfEmpty("hack.cheat-indicator.position", 0);
        config::setIfEmpty("hack.cheat-indicator.opacity", 0.5f);
        config::setIfEmpty("hack.cheat-indicator.scale", 0.4f);

        // Create a window
        menu::addWindow("Labels", [&]() {
            if (gui::checkbox("Show Labels", "hack.labels.enabled")) {
                for (auto &container: CONTAINERS) {
                    container.setVisibility(config::get<bool>("hack.labels.enabled"));
                }
            }
            gui::tooltip("Toggle the visibility of labels.");
            menu::keybinds::addMenuKeybind("labels.enabled", "Show Labels", []() {
                bool enabled = !config::get<bool>("hack.labels.enabled");
                config::set("hack.labels.enabled", enabled);
                for (auto &container: CONTAINERS) {
                    container.setVisibility(enabled);
                }
            });

            gui::width(80);
            gui::inputFloat("Padding", "hack.labels.padding", 0.0f, 100.0f);
            gui::tooltip("Controls the padding from the edge of the screen.");
            gui::width();

            ImGui::Separator();

            gui::toggleSetting("Cheat Indicator", "hack.cheat-indicator.enabled", [&]() {
                if (gui::combo("Position", "hack.cheat-indicator.position", CONTAINER_POSITIONS, 6))
                    respawnLabels();
                gui::tooltip("The position of the cheat indicator on the screen.");
                gui::inputFloat("Opacity", "hack.cheat-indicator.opacity", 0.0f, 1.0f);
                gui::tooltip("The opacity of the cheat indicator.");
                gui::inputFloat("Scale", "hack.cheat-indicator.scale", 0.1f, 2.0f);
                gui::tooltip("The scale of the cheat indicator.");
            }, ImVec2(0, 0), 150);

            ImGui::Separator();

            gui::text("Add Label");
            gui::tooltip("To add a label, select a preset and click the Add button.\n"
                         "You can then customize the label to your liking.");
            gui::width(150);
            gui::combo("##Preset", &s_currentPreset, PRESET_NAMES, PRESET_COUNT);
            gui::width();
            ImGui::SameLine(0, 4);
            if (gui::button("Add")) {
                auto preset = PRESETS[s_currentPreset];
                // If we have at least one label, copy some settings from the last one
                if (!s_labels.empty()) {
                    auto &lastLabel = s_labels.back();
                    preset.font = lastLabel.font;
                    preset.color = lastLabel.color;
                    preset.scale = lastLabel.scale;
                }
                s_labels.push_back(preset);
                respawnLabels();
            }

            ImGui::Separator();

            int toRemove = -1;
            int i = 0;
            for (auto &labelConfig: s_labels) {
                gui::toggleSetting(fmt::format("{}##{}", labelConfig.caption, i).c_str(), &labelConfig.visible, [&]() {
                    std::string captionCopy = labelConfig.caption;
                    if (gui::inputText("Caption", &captionCopy, 64, "My Awesome Label",
                                       ImGuiInputTextFlags_EnterReturnsTrue)) {
                        labelConfig.caption = captionCopy;
                    }
                    gui::tooltip("Caption of the label. (Only used to identify the label in the menu.)\n"
                                 "Press Enter after typing to save.");

                    if (gui::combo("Position", reinterpret_cast<int *>(&labelConfig.position), CONTAINER_POSITIONS, 6))
                        respawnLabels();
                    gui::tooltip("The position of the label on the screen.");

                    int32_t currentFont = 0;
                    for (int j = 0; i < FONT_COUNT; j++) {
                        if (FONTS[j] == labelConfig.font) {
                            currentFont = j;
                            break;
                        }
                    }
                    if (gui::combo("Font", &currentFont, FONT_NAMES, FONT_COUNT)) {
                        labelConfig.font = FONTS[currentFont];
                    }
                    gui::tooltip("The font of the label.");

                    gui::colorEdit("Color", &labelConfig.color);
                    gui::tooltip("The color of the label.");
                    gui::inputFloat("Scale", &labelConfig.scale, 0.1f, 2.0f);
                    gui::tooltip("The scale of the label.");
                    gui::inputText("Text", &labelConfig.text, 100, "FPS: {fps}");
                    gui::tooltip("The text of the label. You can use different placeholders like {fps} or {clock}.\n"
                                 "For more info, visit the GitHub page.");

                    if (gui::button("Remove")) {
                        toRemove = i;
                    }
                    gui::tooltip("Delete this label.");

                    if (gui::button("Move Up", ImVec2(0.5, 0))) {
                        if (i > 0) {
                            std::swap(s_labels[i], s_labels[i - 1]);
                            respawnLabels();
                        }
                    }
                    gui::tooltip("Move this label up in the list.");
                    ImGui::SameLine(0, 4);
                    if (gui::button("Move Down")) {
                        if (i < s_labels.size() - 1) {
                            std::swap(s_labels[i], s_labels[i + 1]);
                            respawnLabels();
                        }
                    }
                    gui::tooltip("Move this label down in the list.");
                }, ImVec2(0, 0), 180);
                i++;
            }

            if (toRemove != -1) {
                s_labels.erase(s_labels.begin() + toRemove);
                respawnLabels();
            }
        });

        // Initialize keybind
        menu::keybinds::setKeybindCallback("labels.enabled", []() {
            bool enabled = !config::get<bool>("hack.labels.enabled");
            config::set("hack.labels.enabled", enabled);
            for (auto &container: CONTAINERS) {
                container.setVisibility(enabled);
            }
        });

        // Initialize container visibility
        for (auto &container: CONTAINERS) {
            container.setVisibility(config::get<bool>("hack.labels.enabled"));
        }

        // Load the labels from the config
        s_labels.clear();
    }

    void Labels::update() {
        if (gd::PlayLayer::get() == nullptr) return;

        for (auto &container: CONTAINERS) {
            container.update();
        }
    }

    void Labels::playLayerInit() {
        for (auto &container: CONTAINERS) {
            container.clear();
        }
    }

    void Labels::playLayerLateInit() {
        // Cheat indicator
        auto *cheatLabel = new Label(".", "bigFont.fnt");
        cheatLabel->multiplyHeight(0.37f);
        cheatLabel->setId("openhack-cheat-indicator");
        auto ci_pos = config::get<int>("hack.cheat-indicator.position");
        CONTAINERS[ci_pos].addLabel(cheatLabel, [](Label *label) {
            auto ci_opacity = config::get<float>("hack.cheat-indicator.opacity");
            auto ci_scale = config::get<float>("hack.cheat-indicator.scale");
            auto is_cheating = config::getGlobal<bool>("isCheating", false);
            label->setVisible(config::get<bool>("hack.cheat-indicator.enabled"), false);
            if (is_cheating) {
                label->setTextColor({1.0f, 0.0f, 0.0f, ci_opacity}, false);
            } else {
                label->setTextColor({0.0f, 1.0f, 0.0f, ci_opacity}, false);
            }
            label->setScale(ci_scale);
        });

        // Add labels
        int i = 1;
        for (auto &labelConfig: s_labels) {
            auto *newLabel = new Label(labelConfig.text, labelConfig.font);
            newLabel->setId(fmt::format("openhack-label{}", i++));
            auto *cfg = &labelConfig;
            newLabel->setFont(cfg->font, false);
            newLabel->setText(cfg->text, false);
            newLabel->setTextColor(cfg->color, false);
            newLabel->setScale(cfg->scale, false);
            newLabel->setVisible(cfg->visible);
            CONTAINERS[static_cast<int>(labelConfig.position)].addLabel(newLabel, [cfg](Label *label) {
                label->setFont(cfg->font, false);
                label->setText(replaceTokens(cfg->text, gd::PlayLayer::get(), nullptr), false);
                label->setTextColor(cfg->color, false);
                label->setScale(cfg->scale, false);
                label->setVisible(cfg->visible);
            });
        }

        for (auto &container: CONTAINERS) {
            container.recreateLabels();
        }
    }

    inline bool isRobTopLevel(gd::GJGameLevel *level) {
        int id = level->m_levelID().value();
        return (id > 0 && id < 100) || (id >= 3001 && id <= 6000);
    }

    inline std::string getDifficultyAsset(gd::GJGameLevel *level) {
        if (level->m_autoLevel()) return "auto";

        if (level->m_ratingsSum() != 0)
            level->m_difficulty(static_cast<gd::GJDifficulty>(level->m_ratingsSum() / 10));

        if (level->isDemon()) {
            switch (level->m_demonDifficulty()) {
                case 3:
                    return "easy_demon";
                case 4:
                    return "medium_demon";
                case 5:
                    return "insane_demon";
                case 6:
                    return "extreme_demon";
                default:
                    return "hard_demon";
            }
        }

        switch (level->m_difficulty()) {
            case gd::GJDifficulty::Easy:
                return "easy";
            case gd::GJDifficulty::Normal:
                return "normal";
            case gd::GJDifficulty::Hard:
                return "hard";
            case gd::GJDifficulty::Harder:
                return "harder";
            case gd::GJDifficulty::Insane:
                return "insane";
            case gd::GJDifficulty::Demon:
                return "hard_demon";
            default:
                return "na";
        }
    }

    std::string
    Labels::replaceTokens(const std::string &text, gd::PlayLayer *playLayer, gd::LevelEditorLayer *editorLayer) {
        // {username} - username of the player
        // {id} - level id
        // {name} - level name
        // {author} - level author
        // {difficulty} - difficulty name (also for image key)
        // {progress} - current progress in the level
        // {best} - best progress in the level
        // {objects} - number of objects in the level
        // {stars} - stars of the level
        // {attempts} - attempts in the level
        // {star_emoji} - places ⭐ or 🌙 depending on the level
        std::unordered_map<std::string, std::function<std::string()>> tokens = {
                {"{username}",   []() {
                    auto *gameManager = gd::GameManager::sharedState();
                    return gameManager->m_playerName();
                }},
                {"{id}",         [playLayer, editorLayer]() {
                    gd::GJBaseGameLayer *layer = playLayer ? (gd::GJBaseGameLayer *) playLayer
                                                           : (gd::GJBaseGameLayer *) editorLayer;
                    if (!layer) return std::string("");
                    return std::to_string(layer->m_level()->m_levelID().value());
                }},
                {"{name}",       [playLayer, editorLayer]() {
                    gd::GJBaseGameLayer *layer = playLayer ? (gd::GJBaseGameLayer *) playLayer
                                                           : (gd::GJBaseGameLayer *) editorLayer;
                    if (!layer) return std::string("Unknown");
                    return layer->m_level()->m_levelName();
                }},
                {"{author}",     [playLayer, editorLayer]() {
                    gd::GJBaseGameLayer *layer = playLayer ? (gd::GJBaseGameLayer *) playLayer
                                                           : (gd::GJBaseGameLayer *) editorLayer;
                    if (!layer) return std::string("Unknown");
                    if (isRobTopLevel(layer->m_level())) return std::string("RobTop");
                    return layer->m_level()->m_creatorName();
                }},
                {"{difficulty}", [playLayer]() {
                    if (!playLayer) return std::string("");
                    return getDifficultyAsset(playLayer->m_level());
                }},
                {"{progress}",   [playLayer]() {
                    if (!playLayer) return std::string("");
                    auto progress = playLayer->getCurrentPercentInt();
                    if (progress < 0) progress = 0;
                    return std::to_string(progress);
                }},
                {"{best}",       [playLayer]() {
                    if (!playLayer) return std::string("");
                    if (playLayer->m_level()->isPlatformer()) {
                        int millis = playLayer->m_level()->m_bestTime();
                        if (millis == 0) return std::string("No Best Time");
                        double seconds = millis / 1000.0;
                        if (seconds < 60.0)
                            return fmt::format("{:.3f}", seconds);
                        int minutes = (int) (seconds / 60.0);
                        seconds -= minutes * 60;
                        return fmt::format("{:02d}:{:06.3f}", minutes, seconds);
                    }
                    return std::to_string(playLayer->m_level()->m_normalPercent().value());
                }},
                {"{objects}",    [playLayer, editorLayer]() {
                    if (playLayer) {
                        return std::to_string(playLayer->m_level()->m_objectCount().value());
                    } else if (editorLayer) {
                        return std::to_string(editorLayer->m_objects()->count());
                    }
                    return std::string("");
                }},
                {"{stars}",      [playLayer]() {
                    if (playLayer) {
                        return std::to_string(playLayer->m_level()->m_stars().value());
                    }
                    return std::string("");
                }},
                {"{attempts}",   [playLayer]() {
                    if (!playLayer) return std::string("");
                    return std::to_string(playLayer->m_attempts());
                }},
                {"{rating}",     [playLayer]() {
                    if (!playLayer) return std::string("");
                    return std::to_string(playLayer->m_level()->m_ratingsSum());
                }},
                {"{star_emoji}", [playLayer]() {
                    if (!playLayer) return std::string("");
                    return std::string(playLayer->m_level()->isPlatformer() ? "🌙" : "⭐");
                }},
                {"{clock}",      []() {
                    std::time_t now = std::time(nullptr);
                    std::tm tm{};
                    localtime_s(&tm, &now);
                    return fmt::format("{:02d}:{:02d}:{:02d}", tm.tm_hour, tm.tm_min, tm.tm_sec);
                }},
                {"{fps}",        []() {
                    return std::to_string(static_cast<int>(ImGui::GetIO().Framerate));
                }},
                {"{cps}",        []() {

                    return std::string("0/0");
                }},
                {"{noclip_acc}", []() {

                    return std::string("0.00");
                }},
        };

        std::string result = text;
        for (auto &token: tokens) {
            size_t pos = 0;
            while ((pos = result.find(token.first, pos)) != std::string::npos) {
                result.replace(pos, token.first.length(), token.second());
                pos += token.second().length();
            }
        }
        return result;
    }

}