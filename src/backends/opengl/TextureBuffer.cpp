#include <pch.h>
#include <backends/TextureBuffer.h>

#include <GL/glew.h>
#include <fwd.hpp>

#include <iostream>

namespace pxr
{
    void TextureBuffer::Create(int width, int height, TextureBufferType type, TextureBufferFilterMode filterMode)
    {
        m_Width = width;
        m_Height = height;
        m_Type = type;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        AllocateBuffer();

        GLint glFilterMode = 0;
        switch (filterMode)
        {
        case pxr::TextureBufferFilterMode::Linear:
            glFilterMode = GL_LINEAR;
            break;
        case pxr::TextureBufferFilterMode::Nearest:
            glFilterMode = GL_NEAREST;
            break;
        default:
            std::cout << "TextureBufferFilterMode not supported!" << std::endl;
            break;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        ClearBuffer();

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void TextureBuffer::Destroy()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    glm::u8vec4* TextureBuffer::FetchPixels(int x, int y, int width, int height)
    {
        if (m_Type != TextureBufferType::LDR)
            std::cout << "TextureBuffer::FetchChannels only supported on LDR Textures!" << std::endl;

        glm::u8vec4* pixels = new glm::u8vec4[width * height];
        glGetTextureSubImage(m_RendererID, 0, x, y, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, width * height * 4, pixels);

        return pixels;
    }

    void TextureBuffer::Bind()
    {
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void TextureBuffer::BindUnit(int unit)
    {
        glBindTextureUnit(unit, m_RendererID);
    }

    void TextureBuffer::Resize(int width, int height)
    {
        m_Width = width;
        m_Height = height;

        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        AllocateBuffer();
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void TextureBuffer::AllocateBuffer()
    {
        switch (m_Type)
        {
        case TextureBufferType::LDR:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            break;
        case TextureBufferType::HDR:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr);
            break;
        case TextureBufferType::DepthStencil:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            break;
        default:
            std::cout << "Unknown texture type!" << std::endl;
            break;
        }
    }

    void TextureBuffer::ClearBuffer()
    {
        switch (m_Type)
        {
        case TextureBufferType::LDR:
            glClearTexImage(m_RendererID, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            break;
        case TextureBufferType::HDR:
            glClearTexImage(m_RendererID, 0, GL_RGBA, GL_FLOAT, nullptr);
            break;
        case TextureBufferType::DepthStencil:
            glClearTexImage(m_RendererID, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            break;
        default:
            std::cout << "Unknown texture type!" << std::endl;
            break;
        }
    }
}