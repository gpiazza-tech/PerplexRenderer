#include <pch.h>
#include "TextureAtlas.h"

#include "Texture.h"
#include <rendering/TextureBuffer.h>
#include <util/Util.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <gl/glew.h>

#include <cstdint>
#include <malloc.h>
#include <filesystem>
#include <iostream>
#include <ostream>

namespace pxr
{
	void TextureAtlas::Create(int size, int pixelsPerUnit)
	{
		stbi_set_flip_vertically_on_load(true);

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, (int*)&m_MaxSize);

		m_Size = size;
		if (m_Size > m_MaxSize)
		{
			std::cerr << "Failed to create TextureAtlas with size " << m_Size <<
				" because the max texture size on this system is " << m_MaxSize << std::endl;
			return;
		}

		m_PixelsPerUnit = pixelsPerUnit;
		m_Texture.Create(size, size, TextureBufferType::LDR, TextureBufferFilterMode::Nearest);
		m_Shelves.reserve(10);
	}

	void TextureAtlas::Destroy()
	{
		m_Texture.Destroy();
		m_Shelves.clear();
		m_NextShelf = 0;
	}

	Texture TextureAtlas::AddTexture(const std::filesystem::path& path)
	{
		int width, height, channels;
		std::filesystem::path absolutePath = RelativePath(path);
		uint32_t* rawImage = (uint32_t*)stbi_load(absolutePath.string().c_str(), &width, &height, &channels, 4);

		// Add padding to image
		int paddedWidth = width + 2;
		int paddedHeight = height + 2;
		uint32_t* paddedImage = (uint32_t*)malloc(paddedWidth * paddedHeight * sizeof(uint32_t));

		if (!paddedImage)
			std::cerr << "TextureAtlas::AddTexture: malloc failed!" << std::endl;

		AddPadding(width, height, rawImage, paddedImage);

		// determine position
		int shelfIndex = GetShelfIndex(paddedHeight);
		for (int i = (int)m_Shelves.size(); i <= shelfIndex; i++)
			// start at the shelf size and continue adding until we reach the desired index
		{
			m_Shelves.emplace_back(Shelf(-1, m_PixelsPerUnit * i + m_PixelsPerUnit + 2, 0));
		}
		Shelf& shelf = m_Shelves[shelfIndex];
		if (shelf.Y == -1 || shelf.NextTextureX + paddedWidth >= m_Size) 
			// if shelf does not exist or runs out of room
		{
			shelf = Shelf(m_NextShelf, shelf.Height, 0);
			m_NextShelf += shelf.Height + 1;
		}
		int x = shelf.NextTextureX;
		int y = shelf.Y;
		shelf.NextTextureX += paddedWidth;

		// Push padded image to GPU
		glBindTexture(GL_TEXTURE_2D, m_Texture.GetID());
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, paddedWidth, paddedHeight, GL_RGBA, GL_UNSIGNED_BYTE, paddedImage);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Cleanup
		stbi_image_free((void*)rawImage);
		free((void*)paddedImage);

		// Create Texture to return
		Texture subTexture;

		subTexture.PixelX = x + 1;
		subTexture.PixelY = y + 1;
		subTexture.PixelWidth = width;
		subTexture.PixelHeight = height;

		subTexture.ScaleFactorX = width / (float)m_PixelsPerUnit;
		subTexture.ScaleFactorY = height / (float)m_PixelsPerUnit;

		subTexture.Xmin = (x + 1) / (float)m_Size;
		subTexture.Ymin = (y + 1) / (float)m_Size;
		subTexture.Xmax = (x + width + 1) / (float)m_Size;
		subTexture.Ymax = (y + height + 1) / (float)m_Size;

		return subTexture;
	}

	Texture TextureAtlas::AddTextureAt(const std::filesystem::path& path, int xPos, int yPos)
	{
		int32_t width, height, channels;
		std::filesystem::path absolutePath = RelativePath(path);
		uint32_t* rawImage = (uint32_t*)stbi_load(absolutePath.string().c_str(), &width, &height, &channels, 4);

		// Add padding to image
		int paddedWidth = width + 2;
		int paddedHeight = height + 2;
		uint32_t* paddedImage = (uint32_t*)malloc(paddedWidth * paddedHeight * sizeof(uint32_t));

		if (!paddedImage)
			std::cerr << "TextureAtlas::AddTexture: malloc failed!" << std::endl;

		AddPadding(width, height, rawImage, paddedImage);

		// Push padded image to GPU
		glBindTexture(GL_TEXTURE_2D, m_Texture.GetID());
		glTexSubImage2D(GL_TEXTURE_2D, 0, xPos - 1, yPos - 1, paddedWidth, paddedHeight, GL_RGBA, GL_UNSIGNED_BYTE, paddedImage);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Cleanup
		stbi_image_free((void*)rawImage);
		free((void*)paddedImage);

		// Create Texture to return
		Texture subTexture;

		subTexture.PixelX = xPos;
		subTexture.PixelY = yPos;
		subTexture.PixelWidth = width;
		subTexture.PixelHeight = height;

		subTexture.ScaleFactorX = width / (float)m_PixelsPerUnit;
		subTexture.ScaleFactorY = height / (float)m_PixelsPerUnit;

		subTexture.Xmin = (xPos) / (float)m_Size;
		subTexture.Ymin = (yPos) / (float)m_Size;
		subTexture.Xmax = (xPos + width) / (float)m_Size;
		subTexture.Ymax = (yPos + height) / (float)m_Size;

		return subTexture;
	}

	uint32_t TextureAtlas::GetAtlasTexture() const
	{
		return m_Texture.GetID();
	}

	void TextureAtlas::AddPadding(int width, int height, const uint32_t* img, uint32_t* newImg)
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

	int TextureAtlas::GetShelfIndex(int textureHeight)
	{
		// If p equals m_PixelsPerUnit and i is the index of the vector, then m_Shelves[i] holds the y position for the shelf with a range of (pi+3) to p(i+1)+2
		// Figuring out which index should be returned based on the height is as simple as solving for i for the min value of the range:
		// minHeight = pi+3 -> i = (minHeight - 3) / p
		// integer division will handle the rest:
		 
		return (textureHeight - 3) / m_PixelsPerUnit;
	}
}