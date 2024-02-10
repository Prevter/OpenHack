#include "imgui.hpp"

#include "../../openhack.hpp"

#include <imgui.h>

#include <GL/glew.h>
#include <GL/GL.h>

namespace openhack::imgui
{
    bool m_isInitialized = false;
    std::function<void()> drawCallback = nullptr;
    std::function<bool()> initCallback = nullptr;
    gd::cocos2d::CCTexture2D *fontTexture = nullptr;

    bool isInitialized()
    {
        return m_isInitialized;
    }

    void generateFontsTextures()
    {
        uint8_t *pixels;
        int width, height;

        auto &io = ImGui::GetIO();
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        fontTexture = gd::cocos2d::CCTexture2D::create();
        gd::cocos2d::CCTexture2D::initWithData(
            fontTexture, pixels,
            gd::cocos2d::CCTexture2DPixelFormat::kCCTexture2DPixelFormat_RGBA8888,
            width, height, gd::cocos2d::CCSize(width, height));

        io.Fonts->SetTexID(reinterpret_cast<ImTextureID>(static_cast<std::uintptr_t>(fontTexture->m_uName())));
    }

    void initialize()
    {
        if (m_isInitialized)
            return;

        ImGui::CreateContext();
        m_isInitialized = true;

        if (initCallback && !initCallback())
            deinitialize();

        generateFontsTextures();
    }

    void deinitialize()
    {
        if (!m_isInitialized)
            return;

        ImGui::DestroyContext();
        m_isInitialized = false;
    }

    void setInitCallback(std::function<bool()> callback)
    {
        initCallback = callback;
    }

    void setDrawCallback(std::function<void()> callback)
    {
        drawCallback = callback;
    }

    ImVec2 cocosToFrame(const gd::cocos2d::CCPoint &pos)
    {
        auto *director = gd::cocos2d::CCDirector::sharedDirector();
        auto *glView = gd::cocos2d::CCDirector::getOpenGLView(director);
        const auto frameSize = glView->getFrameSize();
        gd::cocos2d::CCSize winSize;
        gd::cocos2d::CCDirector::getWinSize(director, &winSize);

        return ImVec2(
            pos.x / winSize.width * frameSize.width,
            (1.f - pos.y / winSize.height) * frameSize.height);
    }

    gd::cocos2d::CCPoint frameToCocos(const ImVec2 &pos)
    {
        auto *director = gd::cocos2d::CCDirector::sharedDirector();
        auto *glView = gd::cocos2d::CCDirector::getOpenGLView(director);
        const auto frameSize = glView->getFrameSize();
        gd::cocos2d::CCSize winSize;
        gd::cocos2d::CCDirector::getWinSize(director, &winSize);

        return gd::cocos2d::CCPoint(
            pos.x / frameSize.width * winSize.width,
            (1.f - pos.y / frameSize.height) * winSize.height);
    }

    void newFrame()
    {
        auto &io = ImGui::GetIO();

        auto *director = gd::cocos2d::CCDirector::sharedDirector();
        const auto frameSize = gd::cocos2d::CCDirector::getOpenGLView(director)->getFrameSize();
        gd::cocos2d::CCSize winSize;
        gd::cocos2d::CCDirector::getWinSize(director, &winSize);

        io.DisplaySize = ImVec2(frameSize.width, frameSize.height);
        io.DisplayFramebufferScale = ImVec2(
            winSize.width / frameSize.width,
            winSize.height / frameSize.height);

        float deltaTime = gd::cocos2d::CCDirector::getDeltaTime(director);
        if (deltaTime > 0.0f)
            io.DeltaTime = deltaTime;
        else
            io.DeltaTime = 1.0f / 60.0f;

#ifndef PLATFORM_ANDROID
        auto mousePos = cocosToFrame(openhack::utils::getMousePosition());
        io.AddMousePosEvent(mousePos.x, mousePos.y);
#endif

        // Keyboard modifiers
        // auto* kb = gd::cocos2d::CCDirector::getKeyboardDispatcher(director);
        // io.KeyAlt = kb->getAltKeyPressed() || kb->getCommandKeyPressed(); // look
        // io.KeyCtrl = kb->getControlKeyPressed();
        // io.KeyShift = kb->getShiftKeyPressed();
    }

    void renderFrame()
    {
        auto *drawData = ImGui::GetDrawData();

        glEnable(GL_SCISSOR_TEST);

        GLuint vao = 0;
        GLuint vbos[2] = {0, 0};

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(2, &vbos[0]);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);

        glEnableVertexAttribArray(kCCVertexAttrib_Position);
        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, pos)));

        glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
        glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, uv)));

        glEnableVertexAttribArray(kCCVertexAttrib_Color);
        glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), reinterpret_cast<void *>(offsetof(ImDrawVert, col)));

        auto *shaderCache = gd::cocos2d::CCShaderCache::sharedShaderCache();
        auto *shader = gd::cocos2d::CCShaderCache::programForKey(shaderCache, kCCShader_PositionTextureColor);
        gd::cocos2d::CCGLProgram::use(shader);
        gd::cocos2d::CCGLProgram::setUniformsForBuiltins(shader);

        for (int i = 0; i < drawData->CmdListsCount; ++i)
        {
            auto *list = drawData->CmdLists[i];

            // convert vertex coords to cocos space
            for (int j = 0; j < list->VtxBuffer.size(); j++)
            {
                const auto point = frameToCocos(list->VtxBuffer[j].pos);
                list->VtxBuffer[j].pos = ImVec2(point.x, point.y);
            }

            glBufferData(GL_ARRAY_BUFFER, list->VtxBuffer.Size * sizeof(ImDrawVert), list->VtxBuffer.Data, GL_STREAM_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, list->IdxBuffer.Size * sizeof(ImDrawIdx), list->IdxBuffer.Data, GL_STREAM_DRAW);

            for (auto &cmd : list->CmdBuffer)
            {
                const auto textureID = reinterpret_cast<std::uintptr_t>(cmd.GetTexID());
                gd::cocos2d::ccGLBindTexture2D(static_cast<GLuint>(textureID));

                const auto rect = cmd.ClipRect;
                const auto orig = frameToCocos(ImVec2(rect.x, rect.y));
                const auto end = frameToCocos(ImVec2(rect.z, rect.w));

                if (end.x <= orig.x || end.y >= orig.y)
                    continue;

                auto director = gd::cocos2d::CCDirector::sharedDirector();
                auto glView = gd::cocos2d::CCDirector::getOpenGLView(director);
                glView->setScissorInPoints(orig.x, end.y, end.x - orig.x, orig.y - end.y);

                glDrawElements(GL_TRIANGLES, cmd.ElemCount, GL_UNSIGNED_SHORT, reinterpret_cast<void *>(cmd.IdxOffset * sizeof(ImDrawIdx)));
            }
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glDeleteBuffers(2, &vbos[0]);
        glDeleteVertexArrays(1, &vao);

        glDisable(GL_SCISSOR_TEST);
    }

    void draw()
    {
        if (!m_isInitialized)
            return;

        gd::cocos2d::ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        newFrame();
        ImGui::NewFrame();

        if (drawCallback)
            drawCallback();

        ImGui::Render();
        renderFrame();
    }

    void CCEGLView_toggleFullScreen(std::function<void()> original)
    {
        if (!m_isInitialized)
            return original();

        deinitialize();
        original();
        initialize();
    }

    void CCTouchDispatcher_touches(void *touches, void *event, uint32_t type, std::function<void(void *, void *, uint32_t)> original)
    {
        original(touches, event, type);
    }
}