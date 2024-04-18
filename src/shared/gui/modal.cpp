#include "modal.hpp"

#include <utility>
#include "gui.hpp"

namespace openhack::gui {
    void Modal::create(const std::string &title, std::function<void(Modal *)> onDraw, bool canEscapeClose) {
        // Check if popup with the same title already exists
        auto &popups = getPopups();
        for (auto popup: popups) {
            if (popup->getTitle() == title) {
                return;
            }
        }

        // Create new popup
        auto *popup = new Modal(title, std::move(onDraw), canEscapeClose);
        popups.push_back(popup);
    }

    void Modal::create(const std::string &title, const std::string &message) {
        create(title, [message](Modal *popup) {
            gui::text("%s", message.c_str());
            if (gui::button("OK")) {
                popup->close();
            }
        });
    }

    void Modal::draw() {
        bool isOpen = false;
        ImGui::OpenPopup(m_title.c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup | ImGuiPopupFlags_NoOpenOverItems);

        auto scale = config::getGlobal<float>("UIScale");
        ImGui::SetNextWindowSizeConstraints({MIN_SIZE.x * scale, MIN_SIZE.y * scale},
                                            {MAX_SIZE.x * scale, MAX_SIZE.y * scale});

        auto font = gui::getFont();
        ImGui::PushFont(font.title);

        if (ImGui::BeginPopupModal(m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            isOpen = true;
            if (m_canEscapeClose && utils::isKeyPressed("Esc")) {
                close();
                return ImGui::EndPopup();
            }
            ImGui::PushFont(font.normal);
            m_onDraw(this);
            ImGui::PopFont();
            ImGui::EndPopup();
        }

        ImGui::PopFont();

        // check if the popup was closed
        if (!isOpen) {
            auto &popups = getPopups();
            popups.erase(std::remove(popups.begin(), popups.end(), this), popups.end());
        }
    }

    const std::string &Modal::getTitle() const {
        return m_title;
    }

    void Modal::setTitle(const std::string &title) {
        m_title = title;
    }

    void Modal::close() {
        ImGui::CloseCurrentPopup();
        auto &popups = getPopups();
        popups.erase(std::remove(popups.begin(), popups.end(), this), popups.end());
    }

    std::vector<Modal *> &getPopups() {
        static std::vector<Modal *> popups;
        return popups;
    }

    void drawPopups() {
        for (auto popup: getPopups()) {
            popup->draw();
        }
    }

    void closePopups() {
        for (auto popup: getPopups()) {
            popup->close();
        }
    }
}