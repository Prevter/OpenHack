#include "blur.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <imgui.h>

namespace openhack::menu::blur {
    uint32_t shader = 0;

    void compileShader() {
        const char *vertexShaderSource = R"(
            #version 330 core
            layout (location = 0) in vec2 aPos;
            layout (location = 1) in vec2 aTexCoord;
            out vec2 TexCoord;
            void main() {
                gl_Position = vec4(aPos, 0.0, 1.0);
                TexCoord = aTexCoord;
            }
        )";
        const char *fragmentShaderSource = R"(
            #version 330 core
            in vec2 TexCoord;
            out vec4 FragColor;
            uniform sampler2D screenTexture;
            uniform float blurAmount;
            void main() {
                vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
                vec4 result = vec4(0.0);
                for (int x = -5; x <= 5; x++) {
                    for (int y = -5; y <= 5; y++) {
                        result += texture(screenTexture, TexCoord + vec2(x, y) * texelSize) / 121.0;
                    }
                }
                FragColor = result;
            }
        )";

        uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        shader = glCreateProgram();
        glAttachShader(shader, vertexShader);
        glAttachShader(shader, fragmentShader);
        glLinkProgram(shader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    uint32_t getShader() {
        return shader;
    }

    State state = State::Off;

    void setState(State newState) {
        state = newState;
    }

    State getState() {
        return state;
    }

    uint32_t currentFrame = 0;

    uint32_t getCurrentFrame() {
        // Get the display size
        auto displaySize = ImGui::GetIO().DisplaySize;

        // Create a new texture
        glGenTextures(1, &currentFrame);
        glBindTexture(GL_TEXTURE_2D, currentFrame);

        // Set the texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Create the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei) displaySize.x, (GLsizei) displaySize.y, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);

        // Return the texture handle
        return currentFrame;
    }

    void render() {
        if (state == State::Off) {
            return;
        }
    }
}