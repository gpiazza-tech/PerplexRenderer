#include <pch.h>
#include "SpriteAtlas.h"

#include "Sprite.h"
#include <backends/TextureBuffer.h>
#include <backends/RenderCommands.h>
#include <util/Util.h>
#include <util/Log.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstdint>
#include <filesystem>

namespace pxr
{
	void SpriteAtlas::Create(int width, int height, int pixelsPerUnit, int textureUnit)
	{
		stbi_set_flip_vertically_on_load(true);

		m_MaxSize = RenderCommands::GetMaxTextureSize();

		m_Width = width;
		m_Height = height;

		m_TextureUnit = textureUnit;

		PXR_ASSERT(m_Width < m_MaxSize && m_Height < m_MaxSize, 
			"Failed to create TextureAtlas with size ({1}, {2}) because the max texture size on this system is {3}!",
			m_Width, m_Height, m_MaxSize);

		m_PixelsPerUnit = pixelsPerUnit;
		m_Texture = new TextureBuffer(width, height, TextureBufferType::LDR, TextureBufferFilterMode::Nearest);
		m_Shelves.reserve(10);
	}

	void SpriteAtlas::Destroy()
	{
		delete m_Texture;
		m_Shelves.clear();
		m_NextShelf = 0;
	}

	AddSpriteResult SpriteAtlas::AddSprite(const std::filesystem::path& path)
	{
		AddSpriteResult result;

		int width, height, channels;
		std::filesystem::path absolutePath = RelativePath(path);
		uint32_t* rawImage = (uint32_t*)stbi_load(absolutePath.string().c_str(), &width, &height, &channels, 4);

		// Add padding to image
		int paddedWidth = width + 2;
		int paddedHeight = height + 2;
		uint32_t* paddedImage = (uint32_t*)malloc(paddedWidth * paddedHeight * sizeof(uint32_t));

		PXR_ASSERT(paddedImage, "TextureAtlas::AddTexture: malloc failed!");

		AddPadding(width, height, rawImage, paddedImage);

		// determine position
		int shelfIndex = GetShelfIndex(paddedHeight);
		for (int i = (int)m_Shelves.size(); i <= shelfIndex; i++)
			// start at the shelf size and continue adding until we reach the desired index
		{
			m_Shelves.emplace_back(Shelf(-1, m_PixelsPerUnit * i + m_PixelsPerUnit + 2, 0));
		}
		Shelf& shelf = m_Shelves[shelfIndex];
		if (shelf.Y == -1 || shelf.NextTextureX + paddedWidth >= m_Width)
			// if shelf does not exist or runs out of room
		{
			shelf = Shelf(m_NextShelf, shelf.Height, 0);
			m_NextShelf += shelf.Height + 1;
		}
		int x = shelf.NextTextureX;
		int y = shelf.Y;
		shelf.NextTextureX += paddedWidth;

		if (m_NextShelf > m_MaxSize)
			// Completely out of room, return AddSpriteStatus::Fail
		{
			result.Status = AddSpriteStatus::Fail;
			return result;
		}

		Sprite subTexture = AllocateBuffer(x, y, paddedWidth, paddedHeight, paddedImage);

		// Cleanup
		stbi_image_free((void*)rawImage);
		free((void*)paddedImage);

		result.Sprite = subTexture;
		result.Status = AddSpriteStatus::Success;

		return result;
	}

	void SpriteAtlas::AddPadding(int width, int height, const uint32_t* img, uint32_t* newImg)
	{
		int paddedWidth = width + 2;
		int paddedHeight = height + 2;

		int targetIndex = 0;
		for (int32_t i = 0; i < width * height; i++)
		{
			// Map the original buffer to the larger buffer:
			// 
			//						-------
			//			 -----		|	  |
			//			 | * |		|  *  |
			//			 |***|  ->	| *** |
			//			 | * |		|  *  |
			//			 -----		|	  |
			//			  			-------
			// This should be benchmarked against a 
			// nested for loop approach.
			//
			targetIndex = (i / width * paddedWidth) + i % width + paddedWidth + 1;
			newImg[targetIndex] = img[i];
		}

		for (int32_t i = 0; i < width; i++)
		{
			// Add vertical padding:
			// 
			//			------- 	-------
			//			|     | 	|  *  |
			//			|  *  |		|  *  |
			//			| *** |  ->	| *** |
			//			|  *  |		|  *  |
			//			|     | 	|  *  |
			//			------- 	-------
			//
			newImg[i + 1] = img[i];
			newImg[(paddedWidth * paddedHeight - 2) - i] = img[(width * height - 1) - i];
		}

		for (int32_t i = 0; i < height; i++)
		{
			// Add horizontal padding:
			// 
			//			------- 	-------
			//			|  *  | 	|  *  |
			//			|  *  |		|  *  |
			//			| *** |  ->	|*****|
			//			|  *  |		|  *  |
			//			|  *  | 	|  *  |
			//			------- 	-------
			//
			newImg[paddedWidth + i * paddedWidth] = img[i * width];
			newImg[2 * paddedWidth + i * paddedWidth - 1] = img[width + i * width - 1];
		}

		// Corners
		newImg[0] = img[0];
		newImg[paddedWidth - 1] = img[width - 1];
		newImg[paddedWidth * paddedHeight - 1] = img[width * height - 1];
		newImg[paddedWidth * paddedHeight - paddedWidth] = img[width * height - width];
	}

	int SpriteAtlas::GetShelfIndex(int textureHeight) const
	{
		// If p equals m_PixelsPerUnit and i is the index of the vector, then m_Shelves[i] holds the y position for the shelf with a range of (pi+3) to p(i+1)+2
		// Figuring out which index should be returned based on the height is as simple as solving for i for the min value of the range:
		// minHeight = pi+3 -> i = (minHeight - 3) / p
		// integer division will handle the rest:
		 
		return (textureHeight - 3) / m_PixelsPerUnit;
	}

	Sprite SpriteAtlas::AllocateBuffer(int x, int y, int width, int height, uint32_t* bytes)
	{
		// Push padded image to GPU
		m_Texture->SetPixels(x, y, width, height, (glm::u8vec4*)bytes);

		// Create Sprite to return
		Sprite sprite;

		int visibleWidth = width - 2;
		int visibleHeight = height - 2;

		sprite.PixelX = x + 1;
		sprite.PixelY = y + 1;
		sprite.PixelWidth = visibleWidth;
		sprite.PixelHeight = visibleHeight;

		sprite.ScaleFactorX = visibleWidth / (float)m_PixelsPerUnit;
		sprite.ScaleFactorY = visibleHeight / (float)m_PixelsPerUnit;

		sprite.Xmin = (x + 1) / (float)m_Width;
		sprite.Ymin = (y + 1) / (float)m_Height;
		sprite.Xmax = (x + 1 + visibleWidth) / (float)m_Width;
		sprite.Ymax = (y + 1 + visibleHeight) / (float)m_Height;

		sprite.TextureUnit = m_TextureUnit;

		return sprite;
	}
}