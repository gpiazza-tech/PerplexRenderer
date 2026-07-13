#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

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

        static void BeginFrame(glm::mat4 projection, glm::vec2 pixelResolution, glm::vec4 background);
        static void BeginBatch();
        static void EndBatch();
        static void Flush();

        static void DrawPixel(const glm::vec3& position, const glm::vec4& color = glm::vec4(1.0f), float emission = 0.0f, bool pixelPerfect = true);
        static void DrawSprite(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const glm::vec4& color, float emission, bool pixelPerfect);
        static void DrawSprite(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const Sprite& colorSprite, const Sprite& emissionSprite, const glm::vec4& color, float emission, bool pixelPerfect);
        static void DrawQuad(const glm::mat4& transform, const Sprite& colorSprite, const Sprite& emissionSprite, const glm::vec4& color, float emission);

        static void DrawCircle(glm::vec2 center, float radius, float thickness, glm::vec4 color = glm::vec4{ 1.0f }, float emission = 0.0f, bool pixelPerfect = true);
        static void DrawBox(glm::vec2 center, glm::vec2 bounds, glm::vec4 color = glm::vec4{ 1.0f }, float emission = 0.0f, bool pixelPerfect = true);
        static void DrawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color = glm::vec4{ 1.0f }, float emission = 0.0f, bool pixelPerfect = true);

        static const RenderStats& GetStats();
        static uint32_t GetPixelsPerUnit();
    };
}