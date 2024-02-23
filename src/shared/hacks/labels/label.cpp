#include "label.hpp"

namespace openhack::hacks {

    Label::Label(const std::string &text, const std::string &font) {
        m_label = gd::cocos2d::CCLabelBMFont::create(text.c_str(), font.c_str());
        m_label->setZOrder(1000);
    }

    void Label::setId(const std::string &id) {
#ifdef OPENHACK_GEODE
        reinterpret_cast<cocos2d::CCLabelBMFont*>(m_label)->setID(id);
#endif
    }

    void Label::setText(const std::string &text) {
        m_label->setString(text);
    }

    void Label::setTextColor(const gui::Color &color) {
        // TODO: Fix this for vanilla
#ifdef OPENHACK_GEODE
         auto *label = reinterpret_cast<cocos2d::CCLabelBMFont*>(m_label);
         label->setColor({static_cast<uint8_t>(color.r * 255), static_cast<uint8_t>(color.g * 255), static_cast<uint8_t>(color.b * 255)});
         label->setOpacity(static_cast<uint8_t>(color.a * 255));
#endif
    }

    void Label::setAnchor(const ImVec2 &anchor) {
        m_label->setAnchorPoint({anchor.x, anchor.y});
    }

    void Label::setOffset(const ImVec2 &offset) {
        m_label->setPosition({offset.x, offset.y});
    }

    void Label::setScale(float scale) {
        m_label->setScale(scale);
    }

    void Label::setVisible(bool visible) {
        m_label->setVisible(visible);
    }

    void Label::addToLayer(gd::cocos2d::CCLayer *layer) {
        layer->addChild(m_label);
    }

    void Label::remove(gd::cocos2d::CCLayer *layer) {
        layer->removeChild(m_label);
    }

}