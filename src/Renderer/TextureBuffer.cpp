#include <pch.h>
#include "TextureBuffer.h"

#include "gl/glew.h"

#include <iostream>

namespace pxr
{
    void TextureBuffer::Create(int width, int height, TextureBufferType type)
    {
        m_Width = width;
        m_Height = height;
        m_Type = type;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        AllocateBuffer();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void TextureBuffer::Destroy()
    {
        glDeleteTextures(1, &m_RendererID);
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
}