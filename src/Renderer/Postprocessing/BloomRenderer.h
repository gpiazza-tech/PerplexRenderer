#pragma once

#include "BloomFBO.h"
#include "../Framebuffer.h"

#include <fwd.hpp>

#include <cstdint>

namespace pxr
{
	class Shader;
	struct Texture;

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
		Framebuffer m_PrefilterFBO;

		glm::ivec2 m_SrcViewportSize;
		glm::vec2 m_SrcViewportSizeFloat;

		Shader* m_PrefilterShader = nullptr;
		Shader* m_DownsampleShader = nullptr;
		Shader* m_UpsampleShader = nullptr;
		Shader* m_ScreenShader = nullptr;
	};
}