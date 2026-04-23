#pragma once

#include <backends/BloomFBO.h>
#include <backends/Framebuffer.h>
#include <backends/Shader.h>

#include <glm/fwd.hpp>

#include <cstdint>

namespace pxr
{
	class Shader;
	struct Sprite;

	class BloomRenderer
	{
	public:
		BloomRenderer() {}
		~BloomRenderer() {}

		bool Init(uint32_t windowWidth, uint32_t windowHeight);
		void Destroy();

		void RenderBloomTexture(uint32_t srcTexture, float threshold, float filterRadius);
		uint32_t BloomTexture();

		void Resize(uint32_t width, uint32_t height);
	private:
		void Prefilter(uint32_t srcTexture, float threshold);
		void RenderDownsamples(uint32_t srcTexture);
		void RenderUpsamples(float filterRadius);
		void Combine(uint32_t srcTexture);
	private:
		BloomFBO m_FBO;
		Framebuffer* m_PrefilterFBO;

		glm::ivec2 m_SrcViewportSize;
		glm::vec2 m_SrcViewportSizeFloat;

		Shader m_PrefilterShader;
		Shader m_DownsampleShader;
		Shader m_UpsampleShader;
		Shader m_ScreenShader;
	};
}