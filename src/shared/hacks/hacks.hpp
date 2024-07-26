#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include <utility>
#include "../platform/win32/sigscan.hpp"
#include "../gui/gui.hpp"
#include "../gui/window.hpp"

namespace openhack::hacks {

    /// @brief Base class for all hack components
    class Component {
    public:
        /// @brief Called when the component is initialized
        virtual void onInit() = 0;

        /// @brief Called when the component should be drawn
        virtual void onDraw() = 0;

        /// @brief Get the ID of the component (used for internal identification)
        virtual const std::string &getId() = 0;

        /// @brief Get the name of the component (used for sorting the components)
        virtual const std::string &getName() { return this->getId(); }
    };

    /// @brief A simple text component
    class TextComponent : public Component {
    public:
        /// @brief Construct a new Text Component object
        explicit TextComponent(std::string text) : m_text(std::move(text)) {}

        void onInit() override {}

        /// @brief Draw the text
        void onDraw() override;

        const std::string &getId() override { return m_text; }

    private:
        std::string m_text;
    };

    /// @brief A toggle component for an opcode hack
    class ToggleComponent : public Component {
    public:
        /// @brief Construct a new Toggle Component object
        ToggleComponent(std::string name, std::string id, std::vector<gd::sigscan::Opcode> opcodes)
                : m_name(std::move(name)), m_id(std::move(id)), m_opcodes(std::move(opcodes)) {}

        /// @brief Loads initial state from the configuration and sets the keybinding if needed
        void onInit() override;

        /// @brief Draw the toggle component
        void onDraw() override;

        /// @brief Set description of the component
        void setDescription(const std::string &description) { m_description = description; }

        /// @brief Set the warning state of the component
        void setWarnings(bool hasWarning) { m_hasWarning = hasWarning; }

        /// @brief Set whether this hack is considered a cheat
        void setIsCheat(bool isCheat) { m_cheat = isCheat; }

        /// @brief Check if the hack is enabled
        [[nodiscard]] bool isEnabled() const { return m_enabled; }

        /// @brief Check if the hack is a cheat
        [[nodiscard]] bool isCheat() const { return m_cheat; }

        /// @brief Check if the hack has a warning
        [[nodiscard]] bool hasWarning() const { return m_hasWarning; }

        /// @brief Write the opcodes to the game's memory
        /// @param enable Whether to enable or disable the hack
        /// @return True if the hack was successfully applied
        bool applyPatch(bool enable);

        /// @brief Write the opcodes to the game's memory using the current state
        /// @return True if the hack was successfully applied
        bool applyPatch() { return applyPatch(m_enabled); }

        /// @brief Called when the hack is toggled to apply opcodes and save it to the configuration
        void toggled();

        const std::string &getId() override { return m_id; }

        const std::string &getName() override { return m_name; }

    private:
        std::string m_name;
        std::string m_description;
        std::string m_id;
        std::vector<gd::sigscan::Opcode> m_opcodes;
        bool m_enabled = false;
        bool m_cheat = false;
        bool m_hasWarning = false;
    };

    class EmbeddedHack : public Component {
    public:
        EmbeddedHack(std::string name, std::string id) : m_name(std::move(name)), m_id(std::move(id)) {}

        /// @brief Called when game is fully loaded (can be used if the hack depends on game state)
        virtual void onLateInit() {}
        /// @brief Called every frame to update the hack
        virtual void update() = 0;
        /// @brief Returns whether the hack is cheating
        virtual bool isCheating() { return false; }

        const std::string &getId() override { return m_id; }
        const std::string &getName() override { return m_name; }

    private:
        std::string m_id;
        std::string m_name;
    };

    /// @brief Load all hacks from JSON files and set up the components
    void initialize();

    /// @brief Get all windows
    std::vector<gui::Window> &getWindows();

    /// @brief Get all components
    std::vector<Component *> &getComponents();

    /// @brief Get all hacks
    std::vector<ToggleComponent *> &getHacks();

    /// @brief Get all embedded hacks
    std::vector<EmbeddedHack *> &getEmbeddedHacks();

    /// @brief Get a component by its ID
    /// @param id The ID of the component
    /// @return The component casted to type T or nullptr if not found
    template<typename T>
    inline T *getComponent(const std::string &id) {
        for (auto &component : getComponents()) {
            if (component->getId() == id)
                return dynamic_cast<T *>(component);
        }
        return nullptr;
    }

    /// @brief Get a hack by its ID
    /// @param id The ID of the hack
    /// @return The hack or nullptr if not found
    inline ToggleComponent *getHack(const std::string &id) {
        for (auto &hack : getHacks()) {
            if (hack->getId() == id)
                return hack;
        }
        return nullptr;
    }

    /// @brief Write the opcodes to the game's memory
    /// @param opcode The opcode to apply
    /// @param enable Whether to enable or disable the opcode
    /// @return True if the opcode was successfully applied
    bool applyOpcode(const gd::sigscan::Opcode &opcode, bool enable);

    /// @brief Verify that the opcode has valid original bytes
    /// @param opcode The opcode to verify
    /// @return True if the opcode has valid original bytes
    bool verifyOpcode(const gd::sigscan::Opcode &opcode);
}