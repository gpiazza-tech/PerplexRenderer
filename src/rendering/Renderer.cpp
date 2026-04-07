#include <pch.h>
#include "Renderer.h"

#include <backends/Shader.h>
#include <backends/VertexArray.h>
#include <backends/VertexBuffer.h>
#include <backends/RenderCommands.h>
#include <sprite/Sprite.h>
#include <sprite/SpriteRegistry.h>
#include <util/Util.h>

#include <fwd.hpp>

#include <cstdint>
#include <memory>
#include <backends/IndexBuffer.h>

namespace pxr
{
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

    static RenderStats s_Stats;

    struct RendererData
    {
        std::shared_ptr<VertexArray> QuadVAO = nullptr;
        std::shared_ptr<VertexBuffer> QuadVBO = nullptr;

        uint32_t IndexCount = 0;

        Vertex* QuadBuffer = nullptr;
        Vertex* QuadBufferPtr = nullptr;

        Shader SpriteShader;
        uint32_t PixelsPerUnit;

        glm::mat4 Projection;
    };

    static RendererData s_Data;

    void Renderer::Init(uint32_t pixelsPerUnit)
    {
        s_Data.QuadBuffer = new Vertex[s_MaxVertexCount];
        s_Data.PixelsPerUnit = pixelsPerUnit;

        // Vertex Buffer
        s_Data.QuadVBO = std::make_shared<VertexBuffer>(nullptr, s_MaxVertexCount * sizeof(Vertex), true);
        s_Data.QuadVBO->SetLayout(
            {
                { Type::Float, 3, false }, // Position
                { Type::Float, 4, false }, // Color
                { Type::Float, 2, false }, // Texture Coords
                { Type::Float, 1, false }, // Texture Index
                { Type::Float, 1, false }, // Emission
            });

        // Index Buffer
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

        IndexBuffer ibo(indices, s_MaxIndexCount);

        // Vertex Array
        s_Data.QuadVAO = std::make_shared<VertexArray>();
        s_Data.QuadVAO->AttachBuffers(*s_Data.QuadVBO, ibo);

        // Shader
        s_Data.SpriteShader.Create("res\\shaders\\SpriteVertex.glsl", "res\\shaders\\SpriteFragment.glsl");
        s_Data.SpriteShader.Use();

        int samplers[32];
        for (int i = 0; i < 32; i++)
            samplers[i] = i;
        glm::mat4 transform = glm::mat4(1.0f);
        s_Data.SpriteShader.SetUniformMat4("u_Transform", (float*)&transform);
        s_Data.SpriteShader.SetUniformIntArray("u_Textures", 32, samplers);
        s_Data.SpriteShader.EndUse();

        SpriteRegistry::Init(pixelsPerUnit);
    }

    void Renderer::Shutdown()
    {
        delete[] s_Data.QuadBuffer;
    }

    void Renderer::BeginFrame()
    {
        RenderCommands::EnableAlphaBlending();
        RenderCommands::Clear({ 0.01f, 0.04f, 0.1f, 1.0f });
        RenderCommands::EnableDepthTest();

        s_Stats.Quads = 0;
        s_Stats.DrawCalls = 0;
    }

    void Renderer::BeginBatch(glm::mat4 projection)
    {
        s_Data.QuadBufferPtr = s_Data.QuadBuffer;

        s_Data.Projection = projection;
        glm::mat4 viewProj = projection * glm::mat4(1.0f);
        s_Data.SpriteShader.Use();
        s_Data.SpriteShader.SetUniformMat4("u_ViewProj", (float*)&viewProj);

        SpriteRegistry::Bind();
    }

    void Renderer::EndBatch()
    {
        uint32_t size = (uint8_t*)s_Data.QuadBufferPtr - (uint8_t*)s_Data.QuadBuffer;

        s_Data.QuadVBO->PushData(s_Data.QuadBuffer, size);
    }

    void Renderer::Flush()
    {
        s_Data.SpriteShader.Use();
        RenderCommands::DrawTriangles(*s_Data.QuadVAO, s_Data.IndexCount);
        s_Data.SpriteShader.EndUse();

        s_Data.IndexCount = 0;

        s_Stats.DrawCalls++;
    }

    void Renderer::DrawPixel(const glm::vec2& position, const glm::vec4& color, float emission, bool pixelPerfect)
    {
        DrawQuad(position, glm::vec2(1.0f), color, emission, SpriteRegistry::GetPixelSprite(), pixelPerfect);
    }

    void Renderer::DrawQuad(const glm::vec2& position, const Sprite& sprite)
    {
        DrawQuad(position, glm::vec2(1.0f), glm::vec4(1.0f), 1.0f, sprite, true);
    }

    void Renderer::DrawQuad(const glm::vec2& position, const Sprite& sprite, float emission)
    {
        DrawQuad(position, glm::vec2(1.0f), glm::vec4(1.0f), emission, sprite, true);
    }

    void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float emission, const Sprite& sprite, bool pixelPerfect)
    {
        if (s_Data.IndexCount >= s_MaxIndexCount)
        {
            EndBatch();
            Flush();
            BeginBatch(s_Data.Projection);
        }

        glm::vec2 scaledSize = { size.x * sprite.ScaleFactorX, size.y * sprite.ScaleFactorY };
        glm::vec2 renderPosition = pixelPerfect ? MakePixelPerfect({ position.x, position.y, 0.0f }, s_Data.PixelsPerUnit) : position;

        s_Data.QuadBufferPtr->Position = { renderPosition.x, renderPosition.y, 0.0f };
        s_Data.QuadBufferPtr->Color = color;
        s_Data.QuadBufferPtr->TexCoord = { sprite.Xmin, sprite.Ymin };
        s_Data.QuadBufferPtr->TexIndex = 0;
        s_Data.QuadBufferPtr->Emission = emission;
        s_Data.QuadBufferPtr++;

        s_Data.QuadBufferPtr->Position = { renderPosition.x + scaledSize.x, renderPosition.y, 0.0f };
        s_Data.QuadBufferPtr->Color = color;
        s_Data.QuadBufferPtr->TexCoord = { sprite.Xmax, sprite.Ymin };
        s_Data.QuadBufferPtr->TexIndex = 0;
        s_Data.QuadBufferPtr->Emission = emission;
        s_Data.QuadBufferPtr++;

        s_Data.QuadBufferPtr->Position = { renderPosition.x + scaledSize.x, renderPosition.y + scaledSize.y, 0.0f };
        s_Data.QuadBufferPtr->Color = color;
        s_Data.QuadBufferPtr->TexCoord = { sprite.Xmax, sprite.Ymax };
        s_Data.QuadBufferPtr->TexIndex = 0;
        s_Data.QuadBufferPtr->Emission = emission;
        s_Data.QuadBufferPtr++;

        s_Data.QuadBufferPtr->Position = { renderPosition.x, renderPosition.y + scaledSize.y, 0.0f };
        s_Data.QuadBufferPtr->Color = color;
        s_Data.QuadBufferPtr->TexCoord = { sprite.Xmin, sprite.Ymax };
        s_Data.QuadBufferPtr->TexIndex = 0;
        s_Data.QuadBufferPtr->Emission = emission;
        s_Data.QuadBufferPtr++;

        s_Data.IndexCount += 6;

        s_Stats.Quads++;
    }

    const RenderStats& Renderer::GetStats()
    {
        return s_Stats;
    }
}
