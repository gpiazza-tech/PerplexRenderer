#pragma once

#include "Texture.h"

#include <GLFW/glfw3.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define BREAK() __debugbreak()

namespace fs = std::filesystem;

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar * msg, const void* data);

std::string StringFromFile(const std::filesystem::path & path);

class Renderer
{
public:
    static void Init(uint32_t pixelsPerUnit);
    static void Shutdown();

    static void SetPixelTexture(const Texture& texture);

    static void BeginBatch();
    static void EndBatch();
    static void Flush();

    static void DrawPixel(const glm::vec2& position, const glm::vec4& color = glm::vec4(1.0f), bool pixelPerfect = true);
    static void DrawQuad(const glm::vec2& position, const Texture& texture);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const Texture& texture, bool pixelPerfect);
};