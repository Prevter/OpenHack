#include "startpos_switch.h"
#include "../menu/gui.h"
#include "../menu/keybinds.h"

namespace hacks
{
    StartposSwitcher *StartposSwitcher::instance = nullptr;

    StartposSwitcher::StartposSwitcher()
    {
        instance = this;
    }
    void StartposSwitcher::init() {}
    void StartposSwitcher::late_init() {}

    void StartposSwitcher::draw(bool embedded)
    {
        if (!embedded)
            return;

        gui::ImToggleButton("Startpos Switcher", &m_enabled,
                            [&]()
                            {
                                gui::ImKeybind("Previous Key", &m_prev_keybind, WINDOW_WIDTH, 15, false);
                                gui::ImKeybind("Next Key", &m_next_keybind, WINDOW_WIDTH, 15, false);
                                gui::ImToggleButton("Show Label", &m_show_label, nullptr, WINDOW_WIDTH);
                            });
    }

    void StartposSwitcher::choose_start_pos(int32_t index)
    {
        if (!m_play_layer || m_startpos_objects.empty())
            return;

        size_t startpos_count = m_startpos_objects.size();

        if (index >= (int32_t)startpos_count)
            index = -1; // -1 means no startpos
        else if (index < -1)
            index = startpos_count - 1;

        m_current_index = index;

        // m_play_layer->set_startPosCheckpoint(0);

        if (m_current_index >= 0)
            m_play_layer->setStartPosObject(m_startpos_objects[m_current_index]);
        else
            m_play_layer->setStartPosObject(nullptr);

        // if (m_play_layer->m_isPracticeMode)
        //     m_play_layer->removeAllCheckpoints();

        m_play_layer->resetLevel();
        m_play_layer->startMusic();

        // update label
        // std::string text = fmt::format("{} / {}", m_current_index + 1, startpos_count);
        // m_label->setString(text.c_str());
    }

    void StartposSwitcher::update()
    {
        if (!instance->m_enabled)
            return;

        if (utils::is_key_pressed(m_prev_keybind))
            choose_start_pos(m_current_index - 1);
        else if (utils::is_key_pressed(m_next_keybind))
            choose_start_pos(m_current_index + 1);

        // Temporary, until CCLabelBMFont is fixed
        // use ImGui to draw the label instead
        if (m_startpos_objects.empty() || !m_show_label)
            return;

        size_t startpos_count = m_startpos_objects.size();
        auto screen_size = ImGui::GetIO().DisplaySize;
        std::string text = fmt::format("{} / {}", m_current_index + 1, startpos_count);
        auto text_size = ImGui::CalcTextSize(text.c_str());
        auto pos = ImVec2(screen_size.x / 2 - text_size.x / 2, screen_size.y - text_size.y - 10);
        ImGui::GetForegroundDrawList()->AddText(
            globals::title_font, 20, pos, IM_COL32(255, 255, 255, 255), text.c_str());
    }

    void StartposSwitcher::load(nlohmann::json *data)
    {
        m_enabled = data->value("startpos_switch.enabled", false);
        m_prev_keybind = data->value("startpos_switch.prev_keybind", 81); // Q
        m_next_keybind = data->value("startpos_switch.next_keybind", 69); // E
        m_show_label = data->value("startpos_switch.show_label", true);
    }

    void StartposSwitcher::save(nlohmann::json *data)
    {
        data->emplace("startpos_switch.enabled", m_enabled);
        data->emplace("startpos_switch.prev_keybind", m_prev_keybind);
        data->emplace("startpos_switch.next_keybind", m_next_keybind);
        data->emplace("startpos_switch.show_label", m_show_label);
    }

    bool StartposSwitcher::load_keybind(keybinds::Keybind *keybind)
    {
        if (keybind->id == "startpos_switch")
        {
            keybind->callback = [&]()
            {
                m_enabled = !m_enabled;
            };
            return true;
        }
        return false;
    }

    void StartposSwitcher::playLayer_init(PlayLayer *self, GJGameLevel *level)
    {
        if (!instance || !instance->m_enabled)
            return;

        instance->m_startpos_objects.clear();
        instance->m_play_layer = self;
    }

    void StartposSwitcher::playLayer_lateInit(PlayLayer *self)
    {
        if (!instance || !instance->m_enabled)
            return;

        // sort startpos objects by x position
        std::sort(
            instance->m_startpos_objects.begin(),
            instance->m_startpos_objects.end(),
            [](GameObject *a, GameObject *b)
            {
                return a->m_startPosition.x < b->m_startPosition.x;
            });

        size_t startpos_count = instance->m_startpos_objects.size();
        if (startpos_count > 0)
            instance->m_current_index = startpos_count - 1;

        // create label
        // std::string text = fmt::format("{} / {}", instance->m_current_index + 1, startpos_count);
        // instance->m_label = cocos2d::CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        // instance->m_label->setAnchorPoint({0.5f, 0.95f});
        // instance->m_label->setZOrder(1000);
        // instance->m_label->setScale(0.5f);
        // instance->m_label->setOpacity(180);
        // instance->m_play_layer->addChild(instance->m_label);
    }

    void StartposSwitcher::playLayer_destructor(PlayLayer *self)
    {
        if (!instance)
            return;

        instance->m_play_layer = nullptr;
        instance->m_startpos_objects.clear();
    }

    void StartposSwitcher::playLayer_addObject(PlayLayer *self, GameObject *object)
    {
        if (!instance || !instance->m_enabled)
            return;

        if (object->m_objectID == 31)
        {
            instance->m_startpos_objects.push_back((StartPosObject *)object);
        }
    }
}