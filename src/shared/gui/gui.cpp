#include "gui.hpp"
#include "../openhack.hpp"
#include <imgui.h>

namespace openhack::gui
{
    std::vector<ImFont *> fonts;

    bool init()
    {
        // Load all fonts from "OPENHACK_DIRECTORY/fonts/"
        auto fontDir = utils::getModFontsDirectory();
        L_TRACE("Loading fonts from: {}", fontDir);

        // Check if the directory exists
        if (!std::filesystem::exists(fontDir))
        {
            L_ERROR("Font directory does not exist: {}", fontDir);
            return false;
        }

        ImGuiIO &io = ImGui::GetIO();

        io.IniFilename = nullptr;

        // Scan for fonts
        ImFontConfig font_cfg;
        font_cfg.OversampleH = 3;
        font_cfg.OversampleV = 3;

        for (const auto &entry : std::filesystem::directory_iterator(fontDir))
        {
            if (entry.is_regular_file())
            {
                auto path = entry.path().string();
                auto ext = path.substr(path.find_last_of('.') + 1);
                if (ext == "ttf")
                {
                    auto font = io.Fonts->AddFontFromFileTTF(path.c_str(), 16.0f, &font_cfg);
                    if (font)
                    {
                        fonts.push_back(font);
                        L_INFO("Loaded font: {}", path);
                    }
                    else
                    {
                        L_ERROR("Failed to load font: {}", path);
                    }
                }
            }
        }

        // Set default font
        if (!fonts.empty())
        {
            io.FontDefault = fonts[0];
        }
        else
        {
            L_ERROR("No fonts loaded! Unloading...");
            return false;
        }

        L_INFO("Initialized ImGui");
        return true;
    }

    void draw()
    {
        // Draw ImGui stuff here
        ImGui::ShowDemoWindow();
    }
}