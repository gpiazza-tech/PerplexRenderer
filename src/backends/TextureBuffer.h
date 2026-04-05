#pragma once

#include <fwd.hpp>

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
		TextureBuffer()
			: m_RendererID(0), m_Width(-1), m_Height(-1), m_Type(TextureBufferType::None)
		{
		}
		~TextureBuffer() {}

		void Create(int width, int height, TextureBufferType type, TextureBufferFilterMode filterMode);
		void Destroy();

		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline uint32_t GetID() const { return m_RendererID; }

		glm::u8vec4* FetchPixels(int x, int y, int width, int height);

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