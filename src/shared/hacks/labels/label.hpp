#pragma once

#include "../hacks.hpp"

namespace openhack::hacks {

    /// @brief Wrapper class for a label
    class Label {
    public:
        Label(std::string text, std::string font);

        /// @brief Set the label's identifier.
        /// @note Used only on Geode to identify the label.
        void setId(const std::string &id);

        /// @brief Set the label's text.
        void setText(const std::string &text, bool update = true);

        /// @brief Set the labels's font
        void setFont(const std::string &font, bool update = true);

        /// @brief Set the text's color.
        void setTextColor(const gui::Color &color, bool update = true);

        /// @brief Set an anchor point for the label.
        void setAnchor(const ImVec2 &anchor, bool update = true);

        /// @brief Set the label's position on the screen.
        void setPosition(const ImVec2 &offset, bool update = true);

        /// @brief Set the label's scale
        void setScale(float scale, bool update = true);

        /// @brief Set the label's visibility.
        void setVisible(bool visible, bool update = true);

        /// @brief Get the label's height.
        float getHeight();

        /// @brief Multiply the label's height by a factor. (Used only by the Cheat Indicator)
        void multiplyHeight(float multiplier);

        /// @brief Add the label to a layer.
        void addToLayer(gd::cocos2d::CCLayer *layer);

        /// @brief Remove the label from a layer.
        void remove(gd::cocos2d::CCLayer *layer);

    public:
        /// @brief Get the label's text.
        const std::string &getText() const { return m_text; }

        /// @brief Get the label's font.
        const std::string &getFont() const { return m_font; }

        /// @brief Get the label's identifier.
        const std::string &getId() const { return m_id; }

        /// @brief Get the label's anchor point.
        const ImVec2 &getAnchor() const { return m_anchor; }

        /// @brief Get the label's position.
        const ImVec2 &getPosition() const { return m_position; }

        /// @brief Get the label's scale.
        float getScale() const { return m_scale; }

        /// @brief Get the label's visibility.
        bool isVisible() const { return m_visible; }

        /// @brief Get the label's color.
        const gui::Color &getColor() const { return m_color; }

    private:
        gd::cocos2d::CCLabelBMFont *m_label = nullptr;

        bool m_visible = true;
        ImVec2 m_position = {0, 0};
        ImVec2 m_anchor = {0, 0};
        float m_scale = 1.0f;
        gui::Color m_color = {1.0f, 1.0f, 1.0f, 1.0f};

        std::string m_text;
        std::string m_font;
        std::string m_id; // Geode only

        float m_heightMultiply = 1.0f;

        /// @brief Update the label's properties if they have changed.
        void update();
    };

}