#include <pch.h>
#include "Framebuffer.h"

#include "TextureBuffer.h"
#include "RenderCommands.h"
#include "Shader.h"

namespace fs = std::filesystem;

void Framebuffer::Create(int width, int height, bool hdr)
{
    m_Width = width;
    m_Height = height;
    m_HDR = hdr;

    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    // Color
    TextureBufferType colorTextureType = hdr ? TextureBufferType::HDR : TextureBufferType::LDR;
    m_ColorTexture.Create(width, height, colorTextureType);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTexture.GetID(), 0);

    // Stencil and Depth
    m_DepthStencilTexture.Create(width, height, TextureBufferType::DepthStencil);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthStencilTexture.GetID(), 0);

    // Assert
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Error: Framebuffer is not complete" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_ScreenShader = new Shader("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\ScreenFragment.glsl");
}

void Framebuffer::Destroy()
{
    /*
    delete m_ScreenShader;

    glDeleteRenderbuffers(1, &m_RBO);
    glDeleteTextures(1, &m_Texture);
    glDeleteFramebuffers(1, &m_FBO);
    */
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
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f); // error pink
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_ScreenShader->Use();
    m_ScreenShader->SetUniformInt("u_Texture", 0);
    RenderCommands::DrawScreen();
    m_ScreenShader->EndUse();

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

    m_ScreenShader->Use();
    m_ScreenShader->SetUniformInt("u_Texture", 0);
    RenderCommands::DrawScreen();
    m_ScreenShader->EndUse();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::Resize(int width, int height)
{
    m_Width = width;
    m_Height = height;

    m_ColorTexture.Resize(width, height);
    m_DepthStencilTexture.Resize(width, height);
}
