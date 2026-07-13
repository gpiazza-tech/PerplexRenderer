#include <pxr/pch.h>
#include <pxr/rendering/Renderer.h>

#include <pxr/backends/Shader.h>
#include <pxr/backends/VertexArray.h>
#include <pxr/backends/VertexBuffer.h>
#include <pxr/backends/IndexBuffer.h>
#include <pxr/backends/RenderCommands.h>
#include <pxr/sprite/Sprite.h>
#include <pxr/sprite/SpriteRegistry.h>
#include <pxr/util/Util.h>

#include <glm/fwd.hpp>

#include <cstdint>
#include <memory>
#include <utility>

namespace pxr
{
    static RenderStats s_Stats;

    struct SpriteVertex
    {
        glm::vec3 Position;

        glm::vec4 Color;
        glm::vec2 ColorTexCoord; 
        float ColorTexIndex;

        float Emission;
        glm::vec2 EmissionTexCoord;
        float EmissionTexIndex;
    };

    struct PixelVertex
    {
        glm::vec3 Position;

        glm::vec4 Color;
        float Emission;
    };

    struct LineVertex
    {
        glm::vec3 Position;

        float Slope;
        float YIntercept;

        glm::vec4 Color;
        float Emission;
    };

    struct BoxVertex
    {
        glm::vec3 Position;

        glm::vec2 Center;
        glm::vec2 Bounds;

        glm::vec4 Color;
        float Emission;
    };

    struct CircleVertex
    {
        glm::vec3 Position;

        glm::vec2 Center;
        float Radius;
        float Thickness;

        glm::vec4 Color;
        float Emission;
    };

    struct RenderFrameData
    {
        Sprite PixelSprite{};
        uint32_t PixelsPerUnit{};
        glm::vec2 PixelResolution{};
        glm::mat4 Projection{};
    };

    template<typename Vertex>
    struct RenderPassData
    {
        size_t MaxCount{ 10000 };
        size_t MaxVertexCount{ MaxCount * 4 };
        size_t MaxIndexCount{ MaxCount * 6 };

        std::shared_ptr<VertexArray> VAO{ nullptr };
        std::shared_ptr<VertexBuffer> VBO{ nullptr };

        uint32_t IndexCount{ 0 };

        Vertex* VertexBufferBegin{ nullptr };
        Vertex* VertexBufferCurrent{ nullptr };

        Shader Shader{};
    };

    static RenderFrameData s_FrameData;

    static RenderPassData<SpriteVertex> s_SpriteData;
    static RenderPassData<PixelVertex> s_PixelData;
    static RenderPassData<LineVertex> s_LineData;
    static RenderPassData<BoxVertex> s_BoxData;
    static RenderPassData<CircleVertex> s_CircleData;

    static std::unique_ptr<uint32_t[]> MakeQuadIndexBuffer(size_t indexCount)
    {
        std::unique_ptr<uint32_t[]> indices = std::make_unique_for_overwrite<uint32_t[]>(indexCount);
        uint32_t offset = 0;
        for (size_t i = 0; i < indexCount; i += 6)
        {
            indices[i + 0] = 0 + offset;
            indices[i + 1] = 1 + offset;
            indices[i + 2] = 2 + offset;

            indices[i + 3] = 2 + offset;
            indices[i + 4] = 3 + offset;
            indices[i + 5] = 0 + offset;

            offset += 4;
        }
        return std::move(indices);
    }

    void Renderer::Init(uint32_t pixelsPerUnit)
    {
        RenderCommands::LoadFunctions();
        s_FrameData.PixelsPerUnit = pixelsPerUnit;

        SpriteRegistry::Init(pixelsPerUnit);
        s_FrameData.PixelSprite = SpriteRegistry::GetPixelSprite();

        // SPRITES
        {
            s_SpriteData.VertexBufferBegin = new SpriteVertex[s_SpriteData.MaxVertexCount];

            // Vertex Buffer
            s_SpriteData.VBO = std::make_shared<VertexBuffer>(nullptr, s_SpriteData.MaxVertexCount * sizeof(SpriteVertex), true);
            s_SpriteData.VBO->SetLayout(
                {
                    { Type::Float, 3, false }, // Position

                    { Type::Float, 4, false }, // Color
                    { Type::Float, 2, false }, // Color Texture Coords
                    { Type::Float, 1, false }, // Color Texture Index

                    { Type::Float, 1, false }, // Emission
                    { Type::Float, 2, false }, // Emission Texture Coords
                    { Type::Float, 1, false }, // Emission Texture Index
                });

            // Index Buffer
            std::unique_ptr<uint32_t[]> indices = MakeQuadIndexBuffer(s_SpriteData.MaxIndexCount);
            IndexBuffer ibo(indices.get(), (uint32_t)s_SpriteData.MaxIndexCount);

            // Vertex Array
            s_SpriteData.VAO = std::make_shared<VertexArray>();
            s_SpriteData.VAO->AttachBuffers(*s_SpriteData.VBO, ibo);

            // Shader
            s_SpriteData.Shader.Create("shaders\\SpriteVertex.glsl", "shaders\\SpriteFragment.glsl");
            s_SpriteData.Shader.Use();

            int samplers[32];
            for (int i = 0; i < 32; i++)
                samplers[i] = i;
            glm::mat4 transform = glm::mat4(1.0f);
            s_SpriteData.Shader.SetUniformMat4("u_Transform", (float*)&transform);
            s_SpriteData.Shader.SetUniformIntArray("u_Textures", 32, samplers);
            s_SpriteData.Shader.EndUse();
        }

        // PIXELS
        {
            s_PixelData.VertexBufferBegin = new PixelVertex[s_PixelData.MaxVertexCount];

            // Vertex Buffer
            s_PixelData.VBO = std::make_shared<VertexBuffer>(nullptr, s_PixelData.MaxVertexCount * sizeof(PixelVertex), true);
            s_PixelData.VBO->SetLayout(
                {
                    { Type::Float, 3, false }, // Position

                    { Type::Float, 4, false }, // Color
                    { Type::Float, 1, false }, // Emission
                });

            // Index Buffer
            std::unique_ptr<uint32_t[]> indices = MakeQuadIndexBuffer(s_PixelData.MaxIndexCount);
            IndexBuffer ibo(indices.get(), (uint32_t)s_PixelData.MaxIndexCount);

            // Vertex Array
            s_PixelData.VAO = std::make_shared<VertexArray>();
            s_PixelData.VAO->AttachBuffers(*s_PixelData.VBO, ibo);

            // Shader
            s_PixelData.Shader.Create("shaders\\PixelVertex.glsl", "shaders\\PixelFragment.glsl");
            s_PixelData.Shader.Use();

            glm::mat4 transform = glm::mat4(1.0f);
            s_PixelData.Shader.SetUniformMat4("u_Transform", (float*)&transform);
            s_PixelData.Shader.EndUse();
        }

        // CIRCLES
        {
            s_CircleData.VertexBufferBegin = new CircleVertex[s_LineData.MaxVertexCount];

            // Vertex Buffer
            s_CircleData.VBO = std::make_shared<VertexBuffer>(nullptr, s_LineData.MaxVertexCount * sizeof(CircleVertex), true);
            s_CircleData.VBO->SetLayout(
                {
                    { Type::Float, 3, false }, // Position

                    { Type::Float, 2, false }, // Center
                    { Type::Float, 1, false }, // Radius
                    { Type::Float, 1, false }, // Thickness

                    { Type::Float, 4, false }, // Color
                    { Type::Float, 1, false }, // Emission
                });

            // Index Buffer
            std::unique_ptr<uint32_t[]> indices = MakeQuadIndexBuffer(s_CircleData.MaxIndexCount);
            IndexBuffer ibo(indices.get(), (uint32_t)s_CircleData.MaxIndexCount);

            // Vertex Array
            s_CircleData.VAO = std::make_shared<VertexArray>();
            s_CircleData.VAO->AttachBuffers(*s_CircleData.VBO, ibo);

            // Shader
            s_CircleData.Shader.Create("shaders\\CircleVertex.glsl", "shaders\\CircleFragment.glsl");
            s_CircleData.Shader.Use();

            glm::mat4 transform = glm::mat4(1.0f);
            s_CircleData.Shader.SetUniformMat4("u_Transform", (float*)&transform);
            s_CircleData.Shader.EndUse();
        }

        // BOX
        {
            s_BoxData.VertexBufferBegin = new BoxVertex[s_BoxData.MaxVertexCount];

            // Vertex Buffer
            s_BoxData.VBO = std::make_shared<VertexBuffer>(nullptr, s_BoxData.MaxVertexCount * sizeof(BoxVertex), true);
            s_BoxData.VBO->SetLayout(
                {
                    { Type::Float, 3, false }, // Position

                    { Type::Float, 2, false }, // Center
                    { Type::Float, 2, false }, // Bounds

                    { Type::Float, 4, false }, // Color
                    { Type::Float, 1, false }, // Emission
                });

            // Index Buffer
            std::unique_ptr<uint32_t[]> indices = MakeQuadIndexBuffer(s_BoxData.MaxIndexCount);
            IndexBuffer ibo(indices.get(), (uint32_t)s_BoxData.MaxIndexCount);

            // Vertex Array
            s_BoxData.VAO = std::make_shared<VertexArray>();
            s_BoxData.VAO->AttachBuffers(*s_BoxData.VBO, ibo);

            // Shader
            s_BoxData.Shader.Create("shaders\\BoxVertex.glsl", "shaders\\BoxFragment.glsl");
            s_BoxData.Shader.Use();

            glm::mat4 transform = glm::mat4(1.0f);
            s_BoxData.Shader.SetUniformMat4("u_Transform", (float*)&transform);
            s_BoxData.Shader.EndUse();
        }

        // LINES
        {
            s_LineData.VertexBufferBegin = new LineVertex[s_LineData.MaxVertexCount];

            // Vertex Buffer
            s_LineData.VBO = std::make_shared<VertexBuffer>(nullptr, s_LineData.MaxVertexCount * sizeof(LineVertex), true);
            s_LineData.VBO->SetLayout(
                {
                    { Type::Float, 3, false }, // Position

                    { Type::Float, 1, false }, // Slope
                    { Type::Float, 1, false }, // YIntercept

                    { Type::Float, 4, false }, // Color
                    { Type::Float, 1, false }, // Emission
                });

            // Index Buffer
            std::unique_ptr<uint32_t[]> indices = MakeQuadIndexBuffer(s_LineData.MaxIndexCount);
            IndexBuffer ibo(indices.get(), (uint32_t)s_LineData.MaxIndexCount);

            // Vertex Array
            s_LineData.VAO = std::make_shared<VertexArray>();
            s_LineData.VAO->AttachBuffers(*s_LineData.VBO, ibo);

            // Shader
            s_LineData.Shader.Create("shaders\\LineVertex.glsl", "shaders\\LineFragment.glsl");
            s_LineData.Shader.Use();

            glm::mat4 transform = glm::mat4(1.0f);
            s_LineData.Shader.SetUniformMat4("u_Transform", (float*)&transform);
            s_LineData.Shader.EndUse();
        }
    }

    void Renderer::Shutdown()
    {
        delete[] s_SpriteData.VertexBufferBegin;
        delete[] s_LineData.VertexBufferBegin;
    }

    void Renderer::BeginFrame(glm::mat4 projection, glm::vec2 pixelResolution, glm::vec4 background)
    {
        RenderCommands::EnableAlphaBlending();
        RenderCommands::Clear(background);
        RenderCommands::EnableDepthTest();

        s_FrameData.Projection = projection;
        s_FrameData.PixelResolution = pixelResolution;

        s_Stats.Quads = 0;
        s_Stats.DrawCalls = 0;
    }

    void Renderer::BeginBatch()
    {
        // SPRITE
        {
            s_SpriteData.VertexBufferCurrent = s_SpriteData.VertexBufferBegin;

            glm::mat4 viewProj = s_FrameData.Projection * glm::mat4(1.0f);
            s_SpriteData.Shader.Use();
            s_SpriteData.Shader.SetUniformMat4("u_ViewProj", (float*)&viewProj);
            s_SpriteData.Shader.EndUse();
        }

        // PIXEL
        {
            s_PixelData.VertexBufferCurrent = s_PixelData.VertexBufferBegin;

            glm::mat4 viewProj = s_FrameData.Projection * glm::mat4(1.0f);
            s_PixelData.Shader.Use();
            s_PixelData.Shader.SetUniformMat4("u_ViewProj", (float*)&viewProj);
            s_PixelData.Shader.EndUse();
        }

        // CIRCLE
        {
            s_CircleData.VertexBufferCurrent = s_CircleData.VertexBufferBegin;

            glm::mat4 viewProj = s_FrameData.Projection * glm::mat4(1.0f);
            s_CircleData.Shader.Use();
            s_CircleData.Shader.SetUniformMat4("u_ViewProj", (float*)&viewProj);
            s_CircleData.Shader.SetUniformFloat("u_PixelsPerUnit", (float)s_FrameData.PixelsPerUnit);
            s_CircleData.Shader.EndUse();
        }

        // BOX
        {
            s_BoxData.VertexBufferCurrent = s_BoxData.VertexBufferBegin;

            glm::mat4 viewProj = s_FrameData.Projection * glm::mat4(1.0f);
            s_BoxData.Shader.Use();
            s_BoxData.Shader.SetUniformMat4("u_ViewProj", (float*)&viewProj);
            s_BoxData.Shader.SetUniformFloat("u_PixelsPerUnit", (float)s_FrameData.PixelsPerUnit);
            s_BoxData.Shader.EndUse();
        }

        // LINE
        {
            s_LineData.VertexBufferCurrent = s_LineData.VertexBufferBegin;

            glm::mat4 viewProj = s_FrameData.Projection * glm::mat4(1.0f);
            s_LineData.Shader.Use();
            s_LineData.Shader.SetUniformMat4("u_ViewProj", (float*)&viewProj);
            s_LineData.Shader.SetUniformFloat("u_PixelsPerUnit", (float)s_FrameData.PixelsPerUnit);
            s_LineData.Shader.EndUse();
        }

        SpriteRegistry::Bind();
    }

    void Renderer::EndBatch()
    {
        // SPRITE
        {
            size_t size = (uint8_t*)s_SpriteData.VertexBufferCurrent - (uint8_t*)s_SpriteData.VertexBufferBegin;
            s_SpriteData.VBO->PushData(s_SpriteData.VertexBufferBegin, static_cast<uint32_t>(size));
        }

        // PIXEL
        {
            size_t size = (uint8_t*)s_PixelData.VertexBufferCurrent - (uint8_t*)s_PixelData.VertexBufferBegin;
            s_PixelData.VBO->PushData(s_PixelData.VertexBufferBegin, static_cast<uint32_t>(size));
        }

        // CIRCLE
        {
            size_t size = (uint8_t*)s_CircleData.VertexBufferCurrent - (uint8_t*)s_CircleData.VertexBufferBegin;
            s_CircleData.VBO->PushData(s_CircleData.VertexBufferBegin, static_cast<uint32_t>(size));
        }

        // BOX
        {
            size_t size = (uint8_t*)s_BoxData.VertexBufferCurrent - (uint8_t*)s_BoxData.VertexBufferBegin;
            s_BoxData.VBO->PushData(s_BoxData.VertexBufferBegin, static_cast<uint32_t>(size));
        }

        // LINE
        {
            size_t size = (uint8_t*)s_LineData.VertexBufferCurrent - (uint8_t*)s_LineData.VertexBufferBegin;
            s_LineData.VBO->PushData(s_LineData.VertexBufferBegin, static_cast<uint32_t>(size));
        }
    }

    void Renderer::Flush()
    {
        // SPRITE
        {
            s_SpriteData.Shader.Use();
            RenderCommands::DrawTriangles(*s_SpriteData.VAO, s_SpriteData.IndexCount);
            s_SpriteData.Shader.EndUse();

            s_SpriteData.IndexCount = 0;
            s_Stats.DrawCalls++;
        }

        // PIXEL
        {
            s_PixelData.Shader.Use();
            RenderCommands::DrawTriangles(*s_PixelData.VAO, s_PixelData.IndexCount);
            s_PixelData.Shader.EndUse();

            s_PixelData.IndexCount = 0;
            s_Stats.DrawCalls++;
        }

        // CIRCLE
        {
            s_CircleData.Shader.Use();
            RenderCommands::DrawTriangles(*s_CircleData.VAO, s_CircleData.IndexCount);
            s_CircleData.Shader.EndUse();

            s_CircleData.IndexCount = 0;
            s_Stats.DrawCalls++;
        }

        // BOX
        {
            s_BoxData.Shader.Use();
            RenderCommands::DrawTriangles(*s_BoxData.VAO, s_BoxData.IndexCount);
            s_BoxData.Shader.EndUse();

            s_BoxData.IndexCount = 0;
            s_Stats.DrawCalls++;
        }

        // LINE
        {
            s_LineData.Shader.Use();
            RenderCommands::DrawTriangles(*s_LineData.VAO, s_LineData.IndexCount);
            s_LineData.Shader.EndUse();

            s_LineData.IndexCount = 0;
            s_Stats.DrawCalls++;
        }
    }

    void Renderer::DrawSprite(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const glm::vec4& color, float emission, bool pixelPerfect)
    {
        DrawSprite(position, rotation, size, s_FrameData.PixelSprite, s_FrameData.PixelSprite, color, emission, pixelPerfect);
    }

    void Renderer::DrawSprite(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const Sprite& colorSprite, const Sprite& emissionSprite, const glm::vec4& color, float emission, bool pixelPerfect)
    {
        glm::vec2 scaledSize = { size.x * colorSprite.ScaleFactorX, size.y * colorSprite.ScaleFactorY };
        glm::vec3 renderPosition = pixelPerfect ? MakePixelPerfect(position, s_FrameData.PixelsPerUnit) : position;

        if (colorSprite.PixelWidth % 2 != 0)
            renderPosition.x += 0.5f / s_FrameData.PixelsPerUnit;
        if (colorSprite.PixelHeight % 2 != 0)
            renderPosition.y += 0.5f / s_FrameData.PixelsPerUnit;

        glm::mat4 transform{};
        if (rotation.z != 0.0f)
            transform = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), renderPosition), rotation.z, glm::vec3{ 0.0f, 0.0f, 1.0f }), { scaledSize.x, scaledSize.y, 1.0f });
        else
            transform = glm::scale(glm::translate(glm::mat4(1.0f), renderPosition), { scaledSize.x, scaledSize.y, 1.0f });

        DrawQuad(transform, colorSprite, emissionSprite, color, emission);
    }

    void Renderer::DrawQuad(const glm::mat4& transform, const Sprite& colorSprite, const Sprite& emissionSprite, const glm::vec4& color, float emission)
    {
        if (s_SpriteData.IndexCount >= s_SpriteData.MaxIndexCount)
        {
            EndBatch();
            Flush();
            BeginBatch();
        }

        s_SpriteData.VertexBufferCurrent->Position = transform * glm::vec4{ -0.5f, -0.5f, 0.0f, 1.0f };
        s_SpriteData.VertexBufferCurrent->Color = color;
        s_SpriteData.VertexBufferCurrent->ColorTexCoord = { colorSprite.Xmin, colorSprite.Ymin };
        s_SpriteData.VertexBufferCurrent->ColorTexIndex = (float)colorSprite.TextureUnit;
        s_SpriteData.VertexBufferCurrent->Emission = emission;
        s_SpriteData.VertexBufferCurrent->EmissionTexCoord = { emissionSprite.Xmin, emissionSprite.Ymin };
        s_SpriteData.VertexBufferCurrent->EmissionTexIndex = (float)emissionSprite.TextureUnit;
        s_SpriteData.VertexBufferCurrent++;

        s_SpriteData.VertexBufferCurrent->Position = transform * glm::vec4{ 0.5f, -0.5f, 0.0f, 1.0f };
        s_SpriteData.VertexBufferCurrent->Color = color;
        s_SpriteData.VertexBufferCurrent->ColorTexCoord = { colorSprite.Xmax, colorSprite.Ymin };
        s_SpriteData.VertexBufferCurrent->ColorTexIndex = (float)colorSprite.TextureUnit;
        s_SpriteData.VertexBufferCurrent->Emission = emission;
        s_SpriteData.VertexBufferCurrent->EmissionTexCoord = { emissionSprite.Xmax, emissionSprite.Ymin };
        s_SpriteData.VertexBufferCurrent->EmissionTexIndex = (float)emissionSprite.TextureUnit;
        s_SpriteData.VertexBufferCurrent++;

        s_SpriteData.VertexBufferCurrent->Position = transform * glm::vec4{ 0.5f, 0.5f, 0.0f, 1.0f };
        s_SpriteData.VertexBufferCurrent->Color = color;
        s_SpriteData.VertexBufferCurrent->ColorTexCoord = { colorSprite.Xmax, colorSprite.Ymax };
        s_SpriteData.VertexBufferCurrent->ColorTexIndex = (float)colorSprite.TextureUnit;
        s_SpriteData.VertexBufferCurrent->Emission = emission;
        s_SpriteData.VertexBufferCurrent->EmissionTexCoord = { emissionSprite.Xmax, emissionSprite.Ymax };
        s_SpriteData.VertexBufferCurrent->EmissionTexIndex = (float)emissionSprite.TextureUnit;
        s_SpriteData.VertexBufferCurrent++;

        s_SpriteData.VertexBufferCurrent->Position = transform * glm::vec4{ -0.5f, 0.5f, 0.0f, 1.0f };
        s_SpriteData.VertexBufferCurrent->Color = color;
        s_SpriteData.VertexBufferCurrent->ColorTexCoord = { colorSprite.Xmin, colorSprite.Ymax };
        s_SpriteData.VertexBufferCurrent->ColorTexIndex = (float)colorSprite.TextureUnit;
        s_SpriteData.VertexBufferCurrent->Emission = emission;
        s_SpriteData.VertexBufferCurrent->EmissionTexCoord = { emissionSprite.Xmin, emissionSprite.Ymax };
        s_SpriteData.VertexBufferCurrent->EmissionTexIndex = (float)emissionSprite.TextureUnit;
        s_SpriteData.VertexBufferCurrent++;

        s_SpriteData.IndexCount += 6;

        s_Stats.Quads++;
    }

    void Renderer::DrawPixel(const glm::vec3& position, const glm::vec4& color, float emission, bool pixelPerfect)
    {
        if (s_SpriteData.IndexCount >= s_SpriteData.MaxIndexCount)
        {
            EndBatch();
            Flush();
            BeginBatch();
        }

        const Sprite& pixelSprite = s_FrameData.PixelSprite;
        const float size = pixelSprite.ScaleFactorX;
        glm::vec3 renderPosition = pixelPerfect ? MakePixelPerfect(position, s_FrameData.PixelsPerUnit) : position;

        s_PixelData.VertexBufferCurrent->Position = { renderPosition.x, renderPosition.y, 0.0f };
        s_PixelData.VertexBufferCurrent->Color = color;
        s_PixelData.VertexBufferCurrent->Emission = emission;
        s_PixelData.VertexBufferCurrent++;

        s_PixelData.VertexBufferCurrent->Position = { renderPosition.x + size, renderPosition.y, 0.0f };
        s_PixelData.VertexBufferCurrent->Color = color;
        s_PixelData.VertexBufferCurrent->Emission = emission;
        s_PixelData.VertexBufferCurrent++;

        s_PixelData.VertexBufferCurrent->Position = { renderPosition.x + size, renderPosition.y + size, 0.0f };
        s_PixelData.VertexBufferCurrent->Color = color;
        s_PixelData.VertexBufferCurrent->Emission = emission;
        s_PixelData.VertexBufferCurrent++;

        s_PixelData.VertexBufferCurrent->Position = { renderPosition.x, renderPosition.y + size, 0.0f };
        s_PixelData.VertexBufferCurrent->Color = color;
        s_PixelData.VertexBufferCurrent->Emission = emission;
        s_PixelData.VertexBufferCurrent++;

        s_PixelData.IndexCount += 6;

        s_Stats.Quads++;
    }

    void Renderer::DrawCircle(glm::vec2 center, float radius, float thickness, glm::vec4 color, float emission, bool pixelPerfect)
    {
        if (s_CircleData.IndexCount >= s_CircleData.MaxIndexCount)
        {
            EndBatch();
            Flush();
            BeginBatch();
        }

        float pixelSize{ 1.0f / s_FrameData.PixelsPerUnit };

        if (pixelPerfect)
        {
            radius = MakePixelPerfect(radius, s_FrameData.PixelsPerUnit);
            center = MakePixelPerfect(center, s_FrameData.PixelsPerUnit);
        }

        float bounds = radius + pixelSize;

        s_CircleData.VertexBufferCurrent->Position = { center.x - bounds, center.y - bounds, 0.0f };
        s_CircleData.VertexBufferCurrent->Center = center;
        s_CircleData.VertexBufferCurrent->Radius = radius;
        s_CircleData.VertexBufferCurrent->Thickness = thickness;
        s_CircleData.VertexBufferCurrent->Color = color;
        s_CircleData.VertexBufferCurrent->Emission = emission;
        s_CircleData.VertexBufferCurrent++;

        s_CircleData.VertexBufferCurrent->Position = { center.x + bounds, center.y - bounds, 0.0f };
        s_CircleData.VertexBufferCurrent->Center = center;
        s_CircleData.VertexBufferCurrent->Radius = radius;
        s_CircleData.VertexBufferCurrent->Thickness = thickness;
        s_CircleData.VertexBufferCurrent->Color = color;
        s_CircleData.VertexBufferCurrent->Emission = emission;
        s_CircleData.VertexBufferCurrent++;

        s_CircleData.VertexBufferCurrent->Position = { center.x + bounds, center.y + bounds, 0.0f };
        s_CircleData.VertexBufferCurrent->Center = center;
        s_CircleData.VertexBufferCurrent->Radius = radius;
        s_CircleData.VertexBufferCurrent->Thickness = thickness;
        s_CircleData.VertexBufferCurrent->Color = color;
        s_CircleData.VertexBufferCurrent->Emission = emission;
        s_CircleData.VertexBufferCurrent++;

        s_CircleData.VertexBufferCurrent->Position = { center.x - bounds, center.y + bounds, 0.0f };
        s_CircleData.VertexBufferCurrent->Center = center;
        s_CircleData.VertexBufferCurrent->Radius = radius;
        s_CircleData.VertexBufferCurrent->Thickness = thickness;
        s_CircleData.VertexBufferCurrent->Color = color;
        s_CircleData.VertexBufferCurrent->Emission = emission;
        s_CircleData.VertexBufferCurrent++;

        s_CircleData.IndexCount += 6;

        s_Stats.Quads++;
    }

    void Renderer::DrawBox(glm::vec2 center, glm::vec2 bounds, glm::vec4 color, float emission, bool pixelPerfect)
    {
        if (s_BoxData.IndexCount >= s_BoxData.MaxIndexCount)
        {
            EndBatch();
            Flush();
            BeginBatch();
        }

        float pixelSize{ 1.0f / s_FrameData.PixelsPerUnit };

        if (pixelPerfect)
        {
            bounds = MakePixelPerfect(bounds, s_FrameData.PixelsPerUnit);
            center = MakePixelPerfect(center, s_FrameData.PixelsPerUnit);
        }

        // avoid rendering a box with width or height of 0
        bounds.x = glm::max(abs(bounds.x), pixelSize / 2.0f);
        bounds.y = glm::max(abs(bounds.y), pixelSize / 2.0f);

        glm::vec2 paddedBounds = bounds + glm::vec2{ pixelSize, pixelSize };

        s_BoxData.VertexBufferCurrent->Position = { center.x - paddedBounds.x, center.y - paddedBounds.y, 0.0f };
        s_BoxData.VertexBufferCurrent->Center = center;
        s_BoxData.VertexBufferCurrent->Bounds = bounds;
        s_BoxData.VertexBufferCurrent->Color = color;
        s_BoxData.VertexBufferCurrent->Emission = emission;
        s_BoxData.VertexBufferCurrent++;

        s_BoxData.VertexBufferCurrent->Position = { center.x + paddedBounds.x, center.y - paddedBounds.y, 0.0f };
        s_BoxData.VertexBufferCurrent->Center = center;
        s_BoxData.VertexBufferCurrent->Bounds = bounds;
        s_BoxData.VertexBufferCurrent->Color = color;
        s_BoxData.VertexBufferCurrent->Emission = emission;
        s_BoxData.VertexBufferCurrent++;

        s_BoxData.VertexBufferCurrent->Position = { center.x + paddedBounds.x, center.y + paddedBounds.y, 0.0f };
        s_BoxData.VertexBufferCurrent->Center = center;
        s_BoxData.VertexBufferCurrent->Bounds = bounds;
        s_BoxData.VertexBufferCurrent->Color = color;
        s_BoxData.VertexBufferCurrent->Emission = emission;
        s_BoxData.VertexBufferCurrent++;

        s_BoxData.VertexBufferCurrent->Position = { center.x - paddedBounds.x, center.y + paddedBounds.y, 0.0f };
        s_BoxData.VertexBufferCurrent->Center = center;
        s_BoxData.VertexBufferCurrent->Bounds = bounds;
        s_BoxData.VertexBufferCurrent->Color = color;
        s_BoxData.VertexBufferCurrent->Emission = emission;
        s_BoxData.VertexBufferCurrent++;

        s_BoxData.IndexCount += 6;

        s_Stats.Quads++;
    }

    void Renderer::DrawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color, float emission, bool pixelPerfect)
    {
        float pixelSize{ 1.0f / s_FrameData.PixelsPerUnit };

        if (pixelPerfect)
        {
            start = MakePixelPerfect(start, s_FrameData.PixelsPerUnit);
            end = MakePixelPerfect(end, s_FrameData.PixelsPerUnit);
        }

        // avoid rendering a box with width or height of 0
        if (start.x == end.x)
            end.x += pixelSize;
        if (start.y == end.y)
            end.y += pixelSize;

        glm::vec2 center{ (end.x + start.x) / 2.0f, (end.y + start.y) / 2.0f };
        glm::vec2 bounds = { center.x - start.x, center.y - start.y };
        float slope = (end.y - start.y) / (end.x - start.x);
        float yIntercept = start.y - start.x * slope;

        if (s_LineData.IndexCount >= s_LineData.MaxIndexCount)
        {
            EndBatch();
            Flush();
            BeginBatch();
        }

        s_LineData.VertexBufferCurrent->Position = { center.x - bounds.x, center.y - bounds.y, 0.0f };
        s_LineData.VertexBufferCurrent->Slope = slope;
        s_LineData.VertexBufferCurrent->YIntercept = yIntercept;
        s_LineData.VertexBufferCurrent->Color = color;
        s_LineData.VertexBufferCurrent->Emission = emission;
        s_LineData.VertexBufferCurrent++;

        s_LineData.VertexBufferCurrent->Position = { center.x + bounds.x, center.y - bounds.y, 0.0f };
        s_LineData.VertexBufferCurrent->Slope = slope;
        s_LineData.VertexBufferCurrent->YIntercept = yIntercept;
        s_LineData.VertexBufferCurrent->Color = color;
        s_LineData.VertexBufferCurrent->Emission = emission;
        s_LineData.VertexBufferCurrent++;

        s_LineData.VertexBufferCurrent->Position = { center.x + bounds.x, center.y + bounds.y, 0.0f };
        s_LineData.VertexBufferCurrent->Slope = slope;
        s_LineData.VertexBufferCurrent->YIntercept = yIntercept;
        s_LineData.VertexBufferCurrent->Color = color;
        s_LineData.VertexBufferCurrent->Emission = emission;
        s_LineData.VertexBufferCurrent++;

        s_LineData.VertexBufferCurrent->Position = { center.x - bounds.x, center.y + bounds.y, 0.0f };
        s_LineData.VertexBufferCurrent->Slope = slope;
        s_LineData.VertexBufferCurrent->YIntercept = yIntercept;
        s_LineData.VertexBufferCurrent->Color = color;
        s_LineData.VertexBufferCurrent->Emission = emission;
        s_LineData.VertexBufferCurrent++;

        s_LineData.IndexCount += 6;

        s_Stats.Quads++;
    }

    const RenderStats& Renderer::GetStats()
    {
        return s_Stats;
    }

    uint32_t Renderer::GetPixelsPerUnit()
    {
        return s_FrameData.PixelsPerUnit;
    }
}
