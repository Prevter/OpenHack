#pragma once

#include "../hacks.hpp"
#include "label.hpp"

namespace openhack::hacks {

    enum class ContainerPosition {
        TopLeft,
        Top,
        TopRight,
        BottomLeft,
        Bottom,
        BottomRight
    };

    const char *const CONTAINER_POSITIONS[] = {
            "Top Left", "Top", "Top Right", "Bottom Left", "Bottom", "Bottom Right"
    };

    /// @brief A list of all font files in the game
    const char *const FONTS[] = {
            "bigFont.fnt", "chatFont.fnt", "gjFont01.fnt", "gjFont02.fnt",
            "gjFont03.fnt", "gjFont04.fnt", "gjFont05.fnt", "gjFont06.fnt",
            "gjFont07.fnt", "gjFont08.fnt", "gjFont09.fnt", "gjFont10.fnt",
            "gjFont11.fnt", "gjFont12.fnt", "gjFont13.fnt", "gjFont14.fnt",
            "gjFont15.fnt", "gjFont16.fnt", "gjFont17.fnt", "gjFont18.fnt",
            "gjFont19.fnt", "gjFont20.fnt", "gjFont21.fnt", "gjFont22.fnt",
            "gjFont23.fnt", "gjFont24.fnt", "gjFont25.fnt", "gjFont26.fnt",
            "gjFont27.fnt", "gjFont28.fnt", "gjFont29.fnt", "gjFont30.fnt",
            "gjFont31.fnt", "gjFont32.fnt", "gjFont33.fnt", "gjFont34.fnt",
            "gjFont35.fnt", "gjFont36.fnt", "gjFont37.fnt", "gjFont38.fnt",
            "gjFont39.fnt", "gjFont40.fnt", "gjFont41.fnt", "gjFont42.fnt",
            "gjFont43.fnt", "gjFont44.fnt", "gjFont45.fnt", "gjFont46.fnt",
            "gjFont47.fnt", "gjFont48.fnt", "gjFont49.fnt", "gjFont50.fnt",
            "gjFont51.fnt", "gjFont52.fnt", "gjFont53.fnt", "gjFont54.fnt",
            "gjFont55.fnt", "gjFont56.fnt", "gjFont57.fnt", "gjFont58.fnt",
            "gjFont59.fnt", "goldFont.fnt",
    };

    /// @brief A list of all font names in the game
    const char *const FONT_NAMES[] = {
            "Pusab", "Aller", "FONT01", "FONT02",
            "FONT03", "FONT04", "FONT05", "FONT06",
            "FONT07", "FONT08", "FONT09", "FONT10",
            "FONT11", "Gargle", "Amatic", "Cartwheel",
            "MothproofScript", "LEMON MILK", "LEMON MILK 2", "Minecraft",
            "OptimusPrincepsSemiBold", "Autolova", "Karate", "a Annyeong Haseyo",
            "Ausweis Hollow", "Gypsy Curse", "Magic School Two", "Old English Five",
            "Yeah Papa", "Ninja Naruto", "Metal Lord", "Drip Drop",
            "Electroharmonix", "Aladin", "Creepster", "Call Of Ops Duty",
            "BlocParty", "Astron Boy", "Osaka-Sans Serif", "Some Time Later",
            "Fatboy Slim BLTC BRK", "Moria Citadel", "Rise of Kingdom", "FantaisieArtistique",
            "Edge of the Galaxy", "Wash Your Hand", "Bitwise", "Foul Fiend",
            "Nandaka Western", "Evil Empire", "Comical Cartoon", "Carton Six",
            "aAssassinNinja", "Public Pixel", "New Walt Disney UI", "Random 5",
            "Crafting Lesson", "Game Of Squids", "Monster Game", "Lo-Sumires",
            "Gewtymol", "Gold Pusab",
    };

    const size_t FONT_COUNT = sizeof(FONTS) / sizeof(FONTS[0]);

    /// @brief Configuration for a label (content, font, position, size, etc.).
    /// Used for storing labels in the config.
    struct LabelConfig {
        std::string caption;
        std::string text;
        std::string font = "bigFont.fnt";
        bool visible = true;
        gui::Color color = {1.0f, 1.0f, 1.0f, 0.5f};
        float scale = 0.35f;
        ContainerPosition position = ContainerPosition::TopLeft;
    };

    /// @brief Manages a collection of labels, stacked one on top of the other
    class LabelsContainer {
    public:
        LabelsContainer() = default;

        explicit LabelsContainer(ContainerPosition position) : m_position(position) {}

        /// @brief Adds a label to the container
        /// @param label The label to add
        /// @param update The update function for the label
        void addLabel(Label *label, const std::function<void(Label *)> &update);

        /// @brief Removes a label from the container
        /// @param label The label to remove
        void removeLabel(Label *label);

        /// @brief Recalculates the position of all labels in the container
        void recalculatePositions();

        /// @brief Reconstructs all labels in the container. Should be called once after PlayLayer::init
        void recreateLabels();

        /// @brief Deletes all labels in the container. Should be called once before PlayLayer::init
        void clear();

        /// @brief Same as clear, but also removes the labels from the scene (used for hot reload)
        void deleteFromScene();

        /// @brief Updates all labels in the container
        void update();

        /// @brief Sets the visibility of the container
        void setVisibility(bool visible) { m_visible = visible; }

        /// @brief Sets the position of the container
        /// @param position The position to set
        void setPosition(ContainerPosition position) { m_position = position; }

    private:
        /// @brief A map of labels and their respective update functions
        std::vector<std::pair<Label *, std::function<void(Label *)>>> m_labels;
        /// @brief The position of the container on the screen
        ContainerPosition m_position = ContainerPosition::TopLeft;
        /// @brief Controls whether the container is visible
        bool m_visible = true;
    };

    /// @brief Shows various information on the screen
    class Labels : public EmbeddedHack {
    public:
        Labels() : EmbeddedHack("Labels", "labels") {}

        void onInit() override;

        void onDraw() override {}

        void update() override;

        bool isCheating() override { return false; }

        /// @brief Replaces all {} tokens in a string with their respective values
        static std::string
        replaceTokens(const std::string &text, gd::PlayLayer *playLayer, gd::LevelEditorLayer *editorLayer);

        /// @brief Save the labels to the config
        static void save();

        /// @brief Load the labels from the config
        static void load();

    public:
        /// @brief PlayLayer::init hook
        static void playLayerInit();

        /// @brief PlayLayer::init hook (called after the original function)
        static void playLayerLateInit();
    };

}