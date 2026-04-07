#pragma once

#include <backends/TextureBuffer.h>

#include <cstdint>
#include <vector>

namespace pxr
{
	// This class uses a shelf-based rect packing algorithm to sort textures of
	// different sizes into the atlas. This is easily the most complicated file in this
	// project, so I've included a short explanation of how the algorithm works here:
	// 
	// The atlas will be split into multiple "shelves" or rows, each with a different height range.
	// m_Shelves stores the y locations of the currently active (non-full) shelves,
	// as well as the x location determining where the next texture will be stored on that shelf.
	// 
	// The indicies of m_Shelves correspond to factors of the m_PixelsPerUnit, 
	// If p equals m_PixelsPerUnit and i is the index of the vector, then m_Shelves[i] holds the y position for the shelf with a range of (pi+3) to p(i+1)+2
	// This means if PixelsPerUnit is 16, the shelf at 
	// m_Shelves[0] will have a range of 3-18
	// m_Shelves[1] will have a range of 19-34,
	// m_Shelves[2] will have a range of 35-50, etc...
	// If a shelf becomes full, it will be assigned to m_NextShelf, and m_NextShelf will increase by that shelf's height.
	// Unused shelves are assigned a default y value of -1

	struct Texture;

	struct Shelf
	{
		Shelf(int y, int height, int nextTextureX)
			: Y(y), Height(height), NextTextureX(nextTextureX)
		{ }

		int Y = -1;
		int Height = -1;
		int NextTextureX = 0;
	};

	class TextureAtlas
	{
	public:
		TextureAtlas() = default;
		~TextureAtlas() {};

		Texture AddTexture(const std::filesystem::path& path);
		Texture AddTextureAt(const std::filesystem::path& path, int xPos, int yPos);

		void Create(int size, int pixelsPerUnit);
		void Destroy();

		TextureBuffer* GetTexture() { return m_Texture; }
	private:
		void AddPadding(int width, int height, const uint32_t* img, uint32_t* newImg);
		Texture AllocateBuffer(int x, int y, int width, int height, uint32_t* bytes);
		int GetShelfIndex(int textureHeight) const;
	private:
		TextureBuffer* m_Texture;

		int m_MaxSize;
		int m_Size;
		int m_PixelsPerUnit;

		std::vector<Shelf> m_Shelves;
		int m_NextShelf;

		// TODO: add some kind of free list so that a new texture can fill in the spot of a removed texture
	};
}