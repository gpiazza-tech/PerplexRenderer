#pragma once

#include <cstdint>

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
	{}
	~TextureBuffer() {}

	void Create(int width, int height, TextureBufferType type);
	void Destroy();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline uint32_t GetID() const { return m_RendererID; }

	void Bind();
	void BindUnit(int unit);

	void Resize(int width, int height);
private:
	void AllocateBuffer();
private:
	uint32_t m_RendererID;

	int m_Width;
	int m_Height;
	TextureBufferType m_Type;
};