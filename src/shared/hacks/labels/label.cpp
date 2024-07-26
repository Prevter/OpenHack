#include "label.hpp"

#include <utility>

#ifndef OPENHACK_GEODE

#include <cocos2d.h>

#endif

namespace openhack::hacks {

    Label::Label(std::string text, std::string font) : m_text(std::move(text)), m_font(std::move(font)) {}

    void Label::setId(const std::string &id) {
        m_id = id;
    }

    void Label::setText(const std::string &text, bool callUpdate) {
        m_text = text;
        if (callUpdate) update();
    }

    void Label::setFont(const std::string &font, bool callUpdate) {
        m_font = font;
        if (callUpdate) update();
    }

    void Label::setTextColor(const gui::Color &color, bool callUpdate) {
        m_color = color;
        if (callUpdate) update();
    }

    void Label::setAnchor(const ImVec2 &anchor, bool callUpdate) {
        m_anchor = anchor;
        if (callUpdate) update();
    }

    void Label::setPosition(const ImVec2 &offset, bool callUpdate) {
        m_position = offset;
        if (callUpdate) update();
    }

    void Label::setScale(float scale, bool callUpdate) {
        m_scale = scale;
        if (callUpdate) update();
    }

    void Label::setVisible(bool visible, bool callUpdate) {
        m_visible = visible;
        if (callUpdate) update();
    }

    float Label::getHeight() {
        if (m_label == nullptr) return 0;
        auto *label = reinterpret_cast<cocos2d::CCLabelBMFont *>(m_label);
        return label->getContentSize().height * m_heightMultiply;
    }

    void Label::multiplyHeight(float multiplier) {
        m_heightMultiply = multiplier;
    }

    void Label::addToLayer(cocos2d::CCLayer *layer) {
        m_label = cocos2d::CCLabelBMFont::create(m_text.c_str(), m_font.c_str());
        m_label->setZOrder(1000);
        layer->addChild(m_label);
        update();

#ifdef OPENHACK_GEODE
        m_label->setID(m_id);
#endif
    }

    void Label::remove(cocos2d::CCLayer *layer) {
        layer->removeChild(m_label);
        delete m_label;
        m_label = nullptr;
    }

    void Label::update() {
        if (m_label == nullptr) return;
        m_label->setFntFile(m_font.c_str());
        m_label->setCString(m_text.c_str());
        m_label->setVisible(m_visible);
        m_label->setScale(m_scale);

        if (m_heightMultiply != 1.0f && m_anchor.y == 1.0f) {
            // Calculate the offset to keep the label in the same position
            auto offset = m_label->getContentSize().height - getHeight();
            m_label->setPosition({m_position.x, m_position.y + offset * m_scale});
        } else {
            m_label->setPosition({m_position.x, m_position.y});
        }
        m_label->setAnchorPoint({m_anchor.x, m_anchor.y});
        auto opacity = static_cast<uint8_t>(m_color.a * 255);
        cocos2d::_ccColor3B col = {static_cast<uint8_t>(m_color.r * 255),
                                   static_cast<uint8_t>(m_color.g * 255),
                                   static_cast<uint8_t>(m_color.b * 255)};
        m_label->setColor(col);
        m_label->setOpacity(opacity);
    }

}