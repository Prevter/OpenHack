#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    class Label {
    public:
        Label(const std::string &text, const std::string &font);

        /// @brief Set the label's identifier.
        /// @note Used only on Geode to identify the label.
        void setId(const std::string &id);

        /// @brief Set the label's text.
        void setText(const std::string &text);

        /// @brief Set the text's color.
        void setTextColor(const gui::Color &color);

        /// @brief Set an anchor point for the label.
        void setAnchor(const ImVec2 &anchor);

        /// @brief Set the label's offset from the anchor point.
        void setOffset(const ImVec2 &offset);

        /// @brief Set the label's scale
        void setScale(float scale);

        /// @brief Set the label's visibility.
        void setVisible(bool visible);

        /// @brief Add the label to a layer.
        void addToLayer(gd::cocos2d::CCLayer *layer);

        /// @brief Remove the label from a layer.
        void remove(gd::cocos2d::CCLayer *layer);

    private:
        gd::cocos2d::CCLabelBMFont *m_label;
    };

}