#include <pch.h>
#include <backends/Framebuffer.h>

#include <backends/TextureBuffer.h>
#include <backends/Shader.h>
#include <backends/RenderCommands.h>

#include <GL/glew.h>

#include <cstdint>
#include <iostream>
#include <ostream>

namespace fs = std::filesystem;

namespace pxr
{
    void Framebuffer::Create(int width, int height, bool hdr)
    {
        m_Width = width;
        m_Height = height;
        m_HDR = hdr;

        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        // Color
        TextureBufferType colorTextureType = hdr ? TextureBufferType::HDR : TextureBufferType::LDR;
        m_ColorTexture.Create(width, height, colorTextureType, TextureBufferFilterMode::Linear);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture.GetID(), 0);

        // Stencil and Depth
        m_DepthStencilTexture.Create(width, height, TextureBufferType::DepthStencil, TextureBufferFilterMode::Linear);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthStencilTexture.GetID(), 0);

        // Assert
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Error: Framebuffer is not complete" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_ScreenShader.Create("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\ScreenFragment.glsl");
    }

    void Framebuffer::Destroy()
    {
        m_ScreenShader.Destroy();

        m_ColorTexture.Destroy();
        m_DepthStencilTexture.Destroy();

        glDeleteFramebuffers(1, &m_FBO);
        
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }

    void Framebuffer::BindTexture()
    {
        glBindTexture(GL_TEXTURE_2D, m_ColorTexture.GetID());
    }

    void Framebuffer::DrawTexture(uint32_t texture)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glViewport(0, 0, m_Width, m_Height);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_ScreenShader.Use();
        m_ScreenShader.SetUniformInt("u_Texture", 0);
        RenderCommands::DrawScreen();
        m_ScreenShader.EndUse();

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::DrawToScreen()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ColorTexture.GetID());

        glViewport(0, 0, m_Width, m_Height);
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f); // error pink
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_ScreenShader.Use();
        m_ScreenShader.SetUniformInt("u_Texture", 0);
        RenderCommands::DrawScreen();
        m_ScreenShader.EndUse();

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Framebuffer::Resize(int width, int height)
    {
        m_Width = width;
        m_Height = height;

        m_ColorTexture.Resize(width, height);
        m_DepthStencilTexture.Resize(width, height);
    }
}