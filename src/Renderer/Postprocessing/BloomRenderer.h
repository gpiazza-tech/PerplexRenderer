#pragma once

#include "BloomFBO.h"

class Shader;
class Texture;

class BloomRenderer
{
public:
	BloomRenderer() = default;
	~BloomRenderer() {}

	bool Init(uint32_t windowWidth, uint32_t windowHeight);
	void Destroy();

	void RenderBloomTexture(uint32_t srcTexture, float filterRadius);
	uint32_t BloomTexture();
private:
	void RenderDownsamples(uint32_t srcTexture);
	void RenderUpsamples(float filterRadius);
private:
	BloomFBO m_FBO;
	glm::ivec2 m_SrcViewportSize;
	glm::vec2 m_SrcViewportSizeFloat;
	Shader* m_DownsampleShader;
	Shader* m_UpsampleShader;
};