#pragma once

#include <rendering/Framebuffer.h>

#include <cstdint>

namespace pxr
{
	class Shader;

	class Pixelator
	{
	public:
		Pixelator() = default;
		~Pixelator() {};

		void Init(int outWidth, int outHeight);
		void Destroy();

		void RenderPixelator(uint32_t srcTexture);
		uint32_t PixelatedTexture();

		void Resize(int outWidth, int outHeight);
	private:
		Framebuffer m_FBO;

		int m_Width;
		int m_Height;

		Shader* m_PixelatorShader;
	};
}