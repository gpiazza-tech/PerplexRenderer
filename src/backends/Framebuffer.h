#pragma once

#include "TextureBuffer.h"
#include "Shader.h"

#include <cstdint>

namespace pxr
{
	class Shader;

	class Framebuffer
	{
	public:
		Framebuffer() = default;
		~Framebuffer() {}

		void Create(int width, int height, bool hdr = false);
		void Destroy();

		void Bind();
		void BindTexture();

		void DrawTexture(uint32_t texture);
		void DrawToScreen();
		inline const uint32_t GetTextureID() { return m_ColorTexture->GetID(); }

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }

		void Resize(int width, int height);
	private:
		uint32_t m_FBO;

		int m_Width;
		int m_Height;
		bool m_HDR;

		TextureBuffer* m_ColorTexture;
		TextureBuffer* m_DepthStencilTexture;

		Shader m_ScreenShader;
	};
}