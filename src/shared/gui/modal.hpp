#pragma once

#include <imgui.h>
#include <string>
#include <functional>
#include <vector>

namespace openhack::gui {
    class Modal {
    public:
        inline static const ImVec2 MIN_SIZE{400, 1}; // Minimum popup size
        inline static const ImVec2 MAX_SIZE{600, 800}; // Maximum popup size

        /// @brief Create a new popup with a custom draw function
        /// @param title The title of the popup
        /// @param onDraw The function that draws the popup
        /// @param canEscapeClose Whether the popup can be closed by pressing the Escape key
        static void create(const std::string &title, std::function<void(Modal *)> onDraw, bool canEscapeClose = true);

        /// @brief Create a new popup with a message and an OK button
        /// @param title The title of the popup
        /// @param message The message to display
        static void create(const std::string &title, const std::string &message);

        /// @brief Create a new popup with a message and two buttons
        /// @param title The title of the popup
        /// @param message The message to display
        /// @param firstButton The text of the first button
        /// @param secondButton The text of the second button
        /// @param callback The function that is called when a button is pressed (true for the first button, false for the second button)
        static void create(const std::string &title, const std::string &message, const std::string &firstButton, const std::string &secondButton, const std::function<void(bool)>& callback);

    public:
        Modal(std::string title, std::function<void(Modal *)> onDraw, bool canEscapeClose)
                : m_title(std::move(title)), m_onDraw(std::move(onDraw)), m_canEscapeClose(canEscapeClose) {}

        /// @brief Draws the popup
        void draw();

        /// @brief Get the title of the popup
        [[nodiscard]] const std::string &getTitle() const;

        /// @brief Set the title of the popup
        void setTitle(const std::string &title);

        /// @brief Close the popup
        void close();

    private:
        std::string m_title;
        std::function<void(Modal *)> m_onDraw;
        bool m_canEscapeClose = true; // Whether the popup can be closed by pressing the Escape key
    };

    /// @brief Get all popups
    [[nodiscard]] std::vector<Modal *> &getPopups();

    /// @brief Draw all popups
    void drawPopups();

    /// @brief Close all popups
    void closePopups();
}