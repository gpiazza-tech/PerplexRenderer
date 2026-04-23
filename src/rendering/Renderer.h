#pragma once

#include <glm/fwd.hpp>

#include <cstdint>
#include <intrin.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define BREAK() __debugbreak()

namespace pxr
{
    struct Sprite;
    class TextureRegistry;

    struct RenderStats
    {
        uint32_t Quads = 0;
        uint32_t DrawCalls = 0;
    };

    class Renderer
    {
    public:
        static void Init(uint32_t pixelsPerUnit);
        static void Shutdown();

        static void BeginFrame(const glm::vec4& background);
        static void BeginBatch(glm::mat4 projection);
        static void EndBatch();
        static void Flush();

        static void DrawPixel(const glm::vec2& position, const glm::vec4& color = glm::vec4(1.0f), float emission = 0.0f, bool pixelPerfect = true);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f), float emission = 1.0f);
        static void DrawQuad(const glm::vec2& position, const Sprite& sprite);
        static void DrawQuad(const glm::vec2& position, const Sprite& sprite, float emission);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Sprite& colorSprite, const Sprite& emissionSprite, const glm::vec4& color, float emission, bool pixelPerfect);

        static const RenderStats& GetStats();
        static uint32_t GetPixelsPerUnit();
    };
}