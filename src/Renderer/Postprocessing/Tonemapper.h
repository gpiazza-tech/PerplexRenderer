#pragma once

#include "../Framebuffer.h"

#include <cstdint>

class Shader;

class Tonemapper
{
public:
	Tonemapper() = default;
	~Tonemapper() {}

	void Init(int width, int height);
	void Destroy();

	void RenderTonemap(uint32_t srcTexture);
	uint32_t TonemappedTexture();

	void Resize(int width, int height);
private:
	Framebuffer m_FBO;

	int m_Width;
	int m_Height;

	Shader* m_TonemapShader;
};