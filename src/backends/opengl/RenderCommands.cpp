#include <pxr/pch.h>
#include <pxr/backends/RenderCommands.h>

#include <pxr/backends/ScreenQuad.h>
#include <pxr/backends/VertexArray.h>
#include <pxr/sprite/ImageBuffer.h>

#include <GL/glew.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <cstdint>

namespace pxr
{
    void RenderCommands::LoadFunctions()
    {
        glewInit();
    }

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

    ImageBuffer RenderCommands::FetchFramebufferPixels(uint32_t fbo, uint64_t x, uint64_t y, uint64_t width, uint64_t height)
    {
        ImageBuffer buf{ (size_t)width, (size_t)height };

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height, GL_RGBA, GL_UNSIGNED_BYTE, buf.Data());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return buf;
    }
}