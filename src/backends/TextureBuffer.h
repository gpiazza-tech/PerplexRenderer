#pragma once

#include <glm/fwd.hpp>

#include <cstdint>

namespace pxr
{ 
	enum class TextureBufferFilterMode
	{
		None = 0,
		Linear,
		Nearest
	};

	enum class TextureBufferType
	{
		None = 0,
		LDR,
		HDR,
		DepthStencil
	};

	class TextureBuffer
	{
	public:
		TextureBuffer(int width, int height, TextureBufferType type, TextureBufferFilterMode filterMode);
		~TextureBuffer();

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline uint32_t GetID() const { return m_RendererID; }

		void GetPixels(int x, int y, int width, int height, glm::u8vec4* pixels);
		void SetPixels(int x, int y, int width, int height, glm::u8vec4* pixels);

		void Bind();
		void BindUnit(int unit);

		void Resize(int width, int height);
	private:
		void AllocateBuffer();
		void ClearBuffer();
	private:
		uint32_t m_RendererID;

		int m_Width;
		int m_Height;
		TextureBufferType m_Type;
	};
}