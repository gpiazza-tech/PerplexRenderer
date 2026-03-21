#pragma once

class Shader;

class Framebuffer
{
public:
	Framebuffer() = default;
	Framebuffer(int width, int height);
	~Framebuffer();

	void Bind();
	void BindTexture();

	void DrawTexture(uint32_t texture);
	void DrawToScreen();
	inline const uint32_t GetTextureID() const { return m_Texture; }

	inline const uint32_t GetWidth() const { return m_Width; }
	inline const uint32_t GetHeight() const { return m_Height; }
private:
	uint32_t m_FBO;
	uint32_t m_RBO;
	uint32_t m_Texture;
	uint32_t m_Width;
	uint32_t m_Height;

	Shader* m_ScreenShader;
};