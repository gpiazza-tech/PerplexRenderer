#pragma once

struct Texture;

class TextureAtlas
{
public:
	TextureAtlas() = default;
	~TextureAtlas() {};

	Texture AddTexture(const std::filesystem::path& path);

	void Create(uint32_t size, uint32_t pixelsPerUnit);
	void Destroy();

	uint32_t GetAtlasTexture();
private:
	uint32_t m_Texture;

	uint32_t m_MaxSize;
	uint32_t m_Size;
	uint32_t m_PixelsPerUnit;

	// Keeps track of the offset the next 
	// added texture should be drawn at
	uint32_t m_CurrentX;
	uint32_t m_CurrentY;
};