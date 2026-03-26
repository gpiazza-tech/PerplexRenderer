#pragma once

#include <rendering/TextureBuffer.h>

#include <cstdint>

namespace pxr
{
	struct Texture;

	class TextureAtlas
	{
	public:
		TextureAtlas() = default;
		~TextureAtlas() {};

		Texture AddTexture(const std::filesystem::path& path);
		Texture AddTextureAt(const std::filesystem::path& path, int xPos, int yPos);

		void Create(int size, int pixelsPerUnit);
		void Destroy();

		uint32_t GetAtlasTexture();
	private:
		void AddPadding(int width, int height, const uint32_t* img, uint32_t* newImg);
	private:
		TextureBuffer m_Texture;

		int m_MaxSize;
		int m_Size;
		int m_PixelsPerUnit;

		// Keeps track of the offset the next added texture should be drawn at
		int m_CurrentX;
		int m_CurrentY;
	};
}