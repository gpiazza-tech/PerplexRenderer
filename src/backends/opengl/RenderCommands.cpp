#include <pch.h>
#include <backends/RenderCommands.h>

#include <backends/ScreenQuad.h>
#include <backends/VertexArray.h>

#include <GL/glew.h>
#include <fwd.hpp>
#include <glm.hpp>

#include <cstdint>

namespace pxr
{
    void RenderCommands::DrawScreen()
    {
        glDisable(GL_DEPTH_TEST);

        static ScreenQuad screenQuad;
        screenQuad.Draw();

        glEnable(GL_DEPTH_TEST);
    }

    void RenderCommands::ResizeViewport(int width, int height)
    {
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    }

    void RenderCommands::DrawTriangles(const VertexArray& VAO, uint32_t count)
    {
        VAO.Bind();
        glDrawElements(GL_TRIANGLES, (GLsizei)count, GL_UNSIGNED_INT, nullptr);
        VAO.Unbind();
    }

    void RenderCommands::EnableAlphaBlending()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
    }

    void RenderCommands::Clear(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void RenderCommands::ClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void RenderCommands::ClearDepth()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void RenderCommands::ClearStencil()
    {
        glClear(GL_STENCIL_BUFFER_BIT);
    }

    void RenderCommands::EnableDepthTest()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void RenderCommands::DisableDepthTest()
    {
        glDisable(GL_DEPTH_TEST);
    }

    int RenderCommands::GetMaxTextureSize()
    {
        int maxTextureSize = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, (int*)&maxTextureSize);
        return maxTextureSize;
    }

    int RenderCommands::GetMaxTextureUnits()
    {
       int maxTextureUnits = 0;
       glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
       return maxTextureUnits;
    }
}