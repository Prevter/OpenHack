#include "startpos-switcher.hpp"
#include "../labels/label.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    std::vector<gd::StartPosObject *> StartPosSwitcher::m_startposes;
    int32_t StartPosSwitcher::m_currentIndex;
    Label *m_label;

    void StartPosSwitcher::updateLabel() {
        // Check if PlayLayer is available
        auto *playLayer = gd::PlayLayer::get();
        if (playLayer == nullptr) return;

        // Check if label is available
        if (m_label == nullptr) return;

        // Update label
        bool show = config::get<bool>("hack.startpos_switch.label.show") &&
                    !m_startposes.empty() &&
                    config::get<bool>("hack.startpos_switch.enabled");
        m_label->setVisible(show);
        m_label->setTextColor(config::get<gui::Color>("hack.startpos_switch.label.color"));
        m_label->setScale(config::get<float>("hack.startpos_switch.label.scale"));
        auto winSize = gd::cocos2d::CCDirector::sharedDirector()->getWinSize();
        m_label->setPosition({winSize.width / 2.f, 2.0f});
        m_label->setAnchor({0.5f, 0.0f});
        auto text = fmt::format("{}/{}", m_currentIndex + 1, m_startposes.size());
        m_label->setText(text);
    }

    void StartPosSwitcher::onInit() {
        // Set the default value
        config::setIfEmpty("hack.startpos_switch.enabled", false);
        config::setIfEmpty("hack.startpos_switch.resetCamera", false);
        config::setIfEmpty<std::string>("hack.startpos_switch.nextKey", "E");
        config::setIfEmpty<std::string>("hack.startpos_switch.prevKey", "Q");
        config::setIfEmpty("hack.startpos_switch.label.show", true);
        config::setIfEmpty("hack.startpos_switch.label.color", gui::Color(1.0f, 1.0f, 1.0f, 0.5f));
        config::setIfEmpty("hack.startpos_switch.label.scale", 0.3f);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("startpos_switch.enabled", []() {
            bool enabled = !config::get<bool>("hack.startpos_switch.enabled");
            config::set("hack.startpos_switch.enabled", enabled);
            updateLabel();
        });
    }

    void StartPosSwitcher::onDraw() {
        gui::callback([](){
            gui::tooltip("Allows you to switch between StartPos objects");
            menu::keybinds::addMenuKeybind("startpos_switch.enabled", "StartPos Switcher", [](){
                bool enabled = !config::get<bool>("hack.startpos_switch.enabled");
                config::set("hack.startpos_switch.enabled", enabled);
                updateLabel();
            });
        });
        if (gui::toggleSetting("StartPos Switcher", "hack.startpos_switch.enabled", []() {
            gui::checkbox("Reset camera", "hack.startpos_switch.resetCamera");
            gui::tooltip("Resets the camera position, scale and rotation to the StartPos object.\n"
                         "(Doesn't work well on some levels.)");
            gui::keybind("Previous", "hack.startpos_switch.prevKey");
            gui::keybind("Next", "hack.startpos_switch.nextKey");
            ImGui::Separator();
            gui::width(120);
            if (gui::checkbox("Show label", "hack.startpos_switch.label.show"))
                updateLabel();
            if (gui::colorEdit("Label color", "hack.startpos_switch.label.color"))
                updateLabel();
            if (gui::inputFloat("Label scale", "hack.startpos_switch.label.scale", 0.1f, 1.0f))
                updateLabel();
            gui::width();
        }, ImVec2(0, 0), 220))
            updateLabel();
    }

    void StartPosSwitcher::update() {
        // Get PlayLayer
        auto *playLayer = gd::PlayLayer::get();
        if (playLayer == nullptr) {
            if (m_label != nullptr) {
                delete m_label;
                m_label = nullptr;
            }
            return;
        }

        if (!config::get<bool>("hack.startpos_switch.enabled", false)) return;

        // Check keybinds
        if (utils::isKeyPressed(config::get<std::string>("hack.startpos_switch.nextKey")))
            pickStartPos(playLayer, m_currentIndex + 1);
        else if (utils::isKeyPressed(config::get<std::string>("hack.startpos_switch.prevKey")))
            pickStartPos(playLayer, m_currentIndex - 1);
    }

    void StartPosSwitcher::initLevel() {
        m_startposes.clear();
        delete m_label;
        m_label = nullptr;
    }

    void StartPosSwitcher::lateInitLevel() {
        // Sort startposes by x position
        std::sort(m_startposes.begin(), m_startposes.end(), [](gd::GameObject *a, gd::GameObject *b) {
            return a->m_startPosition().x < b->m_startPosition().x;
        });

        auto *playLayer = gd::PlayLayer::get();
        size_t count = m_startposes.size();
        m_currentIndex = playLayer->m_isTestMode() ? count - 1 : -1;

        // Setup label
        m_label = new Label("", "bigFont.fnt");
        m_label->setId("openhack-startpos-label");
        m_label->addToLayer(gd::PlayLayer::get());
        updateLabel();
    }

    void StartPosSwitcher::addObject(gd::GameObject *object) {
        uint32_t id = object->m_objectID();
        if (id == 31) {
            m_startposes.push_back(reinterpret_cast<gd::StartPosObject *>(object));
        }
    }

    void StartPosSwitcher::resetLevel() {
        if (!config::get<bool>("hack.startpos_switch.enabled", false)) return;

        // Get PlayLayer
        auto *playLayer = gd::PlayLayer::get();
        if (playLayer == nullptr) return;

        updateLabel();

        if (m_currentIndex >= 0 && config::get<bool>("hack.startpos_switch.resetCamera", true))
            playLayer->resetCamera();
    }

    void StartPosSwitcher::pickStartPos(gd::PlayLayer *playLayer, int32_t index) {
        if (m_startposes.empty()) return;

        size_t count = m_startposes.size();

        if (index >= (int32_t) count) {
            index = -1;
        } else if (index < -1) {
            index = (int32_t) count - 1;
        }

        m_currentIndex = index;
        playLayer->m_startPosCheckpoint(nullptr);

        if (index >= 0) {
            playLayer->setStartPosObject(m_startposes[index]);
        } else {
            playLayer->setStartPosObject(nullptr);
        }

        if (playLayer->m_isPracticeMode())
            playLayer->resetLevelFromStart();

        playLayer->resetLevel();
        playLayer->startMusic();

        // Update label
        updateLabel();

        config::setGlobal<int>("bestRun", 0);
    }

}