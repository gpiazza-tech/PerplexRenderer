#include <pch.h>
#include "Framebuffer.h"

#include "RenderCommands.h"
#include "Shader.h"

namespace fs = std::filesystem;

Framebuffer::Framebuffer(int width, int height)
    : m_Width(width), m_Height(height)
{
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    // Colorbuffer texture
    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // attach colorbuffer to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);
    // Render Object
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // attach stencil and depth buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
    // check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Error: Framebuffer is not complete" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_ScreenShader = new Shader("res\\shaders\\ScreenVertex.glsl", "res\\shaders\\ScreenFragment.glsl");
}

Framebuffer::~Framebuffer()
{
    /*delete m_ScreenShader;

    glDeleteRenderbuffers(1, &m_RBO);
    glDeleteTextures(1, &m_Texture);
    glDeleteFramebuffers(1, &m_FBO);*/
}

void Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void Framebuffer::BindTexture()
{
    glBindTexture(GL_TEXTURE_2D, m_Texture);
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
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    glViewport(0, 0, m_Width, m_Height);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f); // error pink
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_ScreenShader->Use();
    m_ScreenShader->SetUniformInt("u_Texture", 0);
    RenderCommands::DrawScreen();
    m_ScreenShader->EndUse();

    glBindTexture(GL_TEXTURE_2D, 0);
}
