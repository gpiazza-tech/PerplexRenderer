#pragma once

#include <GLFW/glfw3.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define BREAK() __debugbreak()

namespace fs = std::filesystem;

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar * msg, const void* data);

std::string StringFromFile(const std::filesystem::path & path);

struct Texture;
class TextureRegistry;

class Renderer
{
public:
    static void Init(uint32_t pixelsPerUnit);
    static void Shutdown();

    static void BeginBatch(glm::mat4 projection);
    static void EndBatch();
    static void Flush();

    static void DrawPixel(const glm::vec2& position, const glm::vec4& color = glm::vec4(1.0f), float emission = 0.0f, bool pixelPerfect = true);
    static void DrawQuad(const glm::vec2& position, const Texture& texture);
    static void DrawQuad(const glm::vec2& position, const Texture& texture, float emission);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float emission, const Texture& texture, bool pixelPerfect);

    static TextureRegistry& GetTextureRegistry();
};