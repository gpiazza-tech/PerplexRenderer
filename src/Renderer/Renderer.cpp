#include <pch.h>
#include "Renderer.h"

#include "Shader.h"
#include "Texture.h"
#include "TextureRegistry.h"
#include "Util.h"

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        std::cout << msg << std::endl;
        BREAK();
    }
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        std::cout << msg << std::endl;
    }
}

static const size_t s_MaxQuadCount = 10000;
static const size_t s_MaxVertexCount = s_MaxQuadCount * 4;
static const size_t s_MaxIndexCount = s_MaxQuadCount * 6;
static const size_t s_MaxTextures = 32;

struct Vertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    float TexIndex;
    float Emission;
};

struct RendererData
{
    uint32_t QuadVAO = 0;
    uint32_t QuadVBO = 0;
    uint32_t QuadIBO = 0;

    uint32_t IndexCount = 0;

    Vertex* QuadBuffer = nullptr;
    Vertex* QuadBufferPtr = nullptr;

    Shader SpriteShader;
    uint32_t PixelsPerUnit;
    Texture PixelTexture;

    glm::mat4 Projection;
};

static RendererData s_Data;
static TextureRegistry s_TextureRegistry;

void Renderer::Init(uint32_t pixelsPerUnit)
{
    s_Data.QuadBuffer = new Vertex[s_MaxVertexCount];

    glCreateVertexArrays(1, &s_Data.QuadVAO);
    glBindVertexArray(s_Data.QuadVAO);

    glCreateBuffers(1, &s_Data.QuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, s_MaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexArrayAttrib(s_Data.QuadVAO, 0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

    glEnableVertexArrayAttrib(s_Data.QuadVAO, 1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));

    glEnableVertexArrayAttrib(s_Data.QuadVAO, 2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord));

    glEnableVertexArrayAttrib(s_Data.QuadVAO, 3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexIndex));

    glEnableVertexArrayAttrib(s_Data.QuadVAO, 4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Emission));

    uint32_t indices[s_MaxIndexCount];
    uint32_t offset = 0;
    for (size_t i = 0; i < s_MaxIndexCount; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;

        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;

        offset += 4;
    }

    glCreateBuffers(1, &s_Data.QuadIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.QuadIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Shader
    s_Data.SpriteShader = Shader("res\\shaders\\SpriteVertex.glsl", "res\\shaders\\SpriteFragment.glsl");
    s_Data.SpriteShader.Use();

    int samplers[32];
    for (int i = 0; i < 32; i++)
        samplers[i] = i;
    glm::mat4 transform = glm::mat4(1.0f);
    s_Data.SpriteShader.SetUniformMat4("u_Transform", (float*)&transform);
    s_Data.SpriteShader.SetUniformIntArray("u_Textures", 32, samplers);
    s_Data.SpriteShader.EndUse();

    s_Data.PixelsPerUnit = pixelsPerUnit;

    s_TextureRegistry.Init();
}

void Renderer::Shutdown()
{
    glDeleteVertexArrays(1, &s_Data.QuadVAO);
    glDeleteBuffers(1, &s_Data.QuadVBO);
    glDeleteBuffers(1, &s_Data.QuadIBO);

    delete[] s_Data.QuadBuffer;
}

void Renderer::SetPixelTexture(const Texture& texture)
{
    s_Data.PixelTexture = texture;
}

void Renderer::BeginBatch(glm::mat4 projection)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    s_Data.QuadBufferPtr = s_Data.QuadBuffer;

    s_Data.Projection = projection;
    glm::mat4 viewProj = projection * glm::mat4(1.0f);
    s_Data.SpriteShader.Use();
    s_Data.SpriteShader.SetUniformMat4("u_ViewProj", (float*)&viewProj);

    s_TextureRegistry.BindToTextureUnits();
}

void Renderer::EndBatch()
{
    GLsizeiptr size = (uint8_t*)s_Data.QuadBufferPtr - (uint8_t*)s_Data.QuadBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_Data.QuadBuffer);
}

void Renderer::Flush()
{
    s_Data.SpriteShader.Use();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.QuadIBO);
    glBindVertexArray(s_Data.QuadVAO);
    glDrawElements(GL_TRIANGLES, s_Data.IndexCount, GL_UNSIGNED_INT, nullptr);
    s_Data.SpriteShader.EndUse();

    s_Data.IndexCount = 0;
}

void Renderer::DrawPixel(const glm::vec2& position, const glm::vec4& color, bool pixelPerfect)
{
    DrawQuad(position, glm::vec2(1.0f), color, 1.0f, s_Data.PixelTexture, pixelPerfect);
}

void Renderer::DrawQuad(const glm::vec2& position, const Texture& texture)
{
    DrawQuad(position, glm::vec2(1.0f), glm::vec4(1.0f), 1.0f, texture, true);
}

void Renderer::DrawQuad(const glm::vec2& position, const Texture& texture, float emission)
{
    DrawQuad(position, glm::vec2(1.0f), glm::vec4(1.0f), emission, texture, true);
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float emission, const Texture& texture, bool pixelPerfect)
{
    if (s_Data.IndexCount >= s_MaxIndexCount)
    {
        EndBatch();
        Flush(); 
        BeginBatch(s_Data.Projection);
    }

    glm::vec2 scaledSize = { size.x * texture.ScaleFactorX, size.y * texture.ScaleFactorY };
    glm::vec2 renderPosition = pixelPerfect ? MakePixelPerfect({ position.x, position.y, 0.0f }, s_Data.PixelsPerUnit) : position;

    s_Data.QuadBufferPtr->Position = { renderPosition.x, renderPosition.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoord = { texture.Xmin, texture.Ymin };
    s_Data.QuadBufferPtr->TexIndex = 0;
    s_Data.QuadBufferPtr->Emission = emission;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { renderPosition.x + scaledSize.x, renderPosition.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoord = { texture.Xmax, texture.Ymin };
    s_Data.QuadBufferPtr->TexIndex = 0;
    s_Data.QuadBufferPtr->Emission = emission;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { renderPosition.x + scaledSize.x, renderPosition.y + scaledSize.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoord = { texture.Xmax, texture.Ymax };
    s_Data.QuadBufferPtr->TexIndex = 0;
    s_Data.QuadBufferPtr->Emission = emission;
    s_Data.QuadBufferPtr++;

    s_Data.QuadBufferPtr->Position = { renderPosition.x, renderPosition.y + scaledSize.y, 0.0f };
    s_Data.QuadBufferPtr->Color = color;
    s_Data.QuadBufferPtr->TexCoord = { texture.Xmin, texture.Ymax };
    s_Data.QuadBufferPtr->TexIndex = 0;
    s_Data.QuadBufferPtr->Emission = emission;
    s_Data.QuadBufferPtr++;

    s_Data.IndexCount += 6;
}

TextureRegistry& Renderer::GetTextureRegistry()
{
    return s_TextureRegistry;
}



