#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <cmath>

namespace openhack::gui {
    struct Color {
        float r, g, b, a;

        Color() : r(0), g(0), b(0), a(1.0f) {}

        Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

        /// @brief Converts the color to ImVec4
        operator ImVec4() const {
            return {r, g, b, a};
        }

        /// @brief Converts the color to ImU32
        operator ImU32() const {
            return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
        }

        /// @brief Returns a pointer to the color data
        /// @return Pointer to the color data
        float *data() {
            return &r;
        }

        /// @brief Creates a new color from HSV values
        /// @param h Hue
        /// @param s Saturation
        /// @param v Value
        /// @param a Alpha
        /// @return New color
        static Color fromHSV(float h, float s, float v, float a = 1.0f) {
            float c = v * s;
            float x = c * (1 - std::abs(fmod(h / 60.0, 2) - 1));
            float m = v - c;

            if (h < 60)
                return {c + m, x + m, m, a};
            if (h < 120)
                return {x + m, c + m, m, a};
            if (h < 180)
                return {m, c + m, x + m, a};
            if (h < 240)
                return {m, x + m, c + m, a};
            if (h < 300)
                return {x + m, m, c + m, a};
            return {c + m, m, x + m, a};
        }

        /// @brief Creates a new color from HSV values
        /// @param hsv HSV values
        /// @return New color
        static Color fromHSV(const ImVec4 &hsv) {
            return fromHSV(hsv.x, hsv.y, hsv.z, hsv.w);
        }

        /// @brief Gets the color from an integer
        /// @param color Integer color
        /// @return New color
        static Color fromInt(int color) {
            return {
                    (float) ((color >> 16) & 0xFF) / 255.0f,
                    (float) ((color >> 8) & 0xFF) / 255.0f,
                    (float) (color & 0xFF) / 255.0f,
                    (float) ((color >> 24) & 0xFF) / 255.0f};
        }

        /// @brief Converts the color to an integer
        /// @return Integer color
        [[nodiscard]] int toInt() const {
            return ((int) (r * 255) << 16) | ((int) (g * 255) << 8) | (int) (b * 255) | ((int) (a * 255) << 24);
        }

        /// @brief Creates a new color from a string
        /// @param color String color in format "RRGGBBAA"
        /// @return New color
        static Color fromString(const char *color) {
            int c;
            sscanf_s(color, "%X", &c);
            return fromInt(c);
        }

        /// @brief Converts the color to a string
        /// @return String color in format "RRGGBBAA"
        [[nodiscard]] std::string toString() const {
            return fmt::format("{:08X}", toInt());
        }
    };

}
