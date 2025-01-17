#pragma once

#include <imgui.h>
#include <string>
#include <functional>
#include "animation/move_action.hpp"

namespace openhack::gui {
    class Window {
    public:
        inline static const ImVec2 MIN_SIZE{220, 1}; // Minimum window size
        inline static const ImVec2 MAX_SIZE{220, 1000}; // Maximum window size

    public:
        /// @brief Create new instance of `Window` with default title and draw callback
        Window() : m_title("Window"), m_isOpen(true), m_position(0, 0), m_drawPosition(0, 0), m_size(MIN_SIZE) {}

        /// @brief Create new instance of `Window` with set title and draw callback
        /// @param title Title of the window
        /// @param onDraw Callback which will be called when the window is drawn
        Window(const std::string &title, std::function<void()> onDraw);

        Window(std::string_view title, std::function<void()> onDraw)
            : Window(std::string(title), onDraw) {}

        /// @brief Draw the window
        void draw();

        /// @brief Check whether the window is opened
        /// @return True if the window is opened
        [[nodiscard]] bool isOpen() const;

        /// @brief Set whether the window is opened
        /// @param open Whether the window should be opened
        void setOpen(bool open);

        /// @brief Check whether the window is located in the visible area
        /// @return True if the window is located in the visible area
        [[nodiscard]] bool isOnScreen() const;

        [[nodiscard]] const std::string &getTitle() const;

        void setTitle(const std::string &title);

        [[nodiscard]] const ImVec2 &getPosition() const;

        void setPosition(const ImVec2 &position);

        [[nodiscard]] const ImVec2 &getDrawPosition() const;

        void setDrawPosition(const ImVec2 &position);

        [[nodiscard]] const ImVec2 &getSize() const;

        void setSize(const ImVec2 &size);

        /// @brief Create new `MoveAction` instance for the window
        /// @param target Target position
        /// @param duration How long the animation should last in seconds
        /// @param easing Easing mode (see "animation/easing.hpp")
        /// @param setRealPosition Whether to change the actual position of the window
        animation::MoveAction *createMoveAction(ImVec2 target, double duration,
                                                animation::EasingFunction easing = animation::easing::easeInOutSine,
                                                bool setRealPosition = false);

    private:
        std::string m_title; // Window title
        bool m_isOpen;       // Whether the window is collapsed or not

        ImVec2 m_position;     // Window position in opened state
        ImVec2 m_drawPosition; // Window position used for drawing (used for animations)
        ImVec2 m_size;         // Window size

        std::function<void()> m_drawCallback; // Callback which will be called when the window is drawn
    };
}