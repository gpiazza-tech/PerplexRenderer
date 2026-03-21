#pragma once

class Shader;
class Framebuffer;

class Tonemapper
{
public:
	Tonemapper() = default;
	~Tonemapper() {}

	void Init(int width, int height);
	void Destroy();

	void RenderTonemap(uint32_t srcTexture);
	uint32_t TonemappedTexture();
private:
	uint32_t m_Texture;
	uint32_t m_FBO;
	Shader* m_TonemapShader;
	int m_Width;
	int m_Height;
};