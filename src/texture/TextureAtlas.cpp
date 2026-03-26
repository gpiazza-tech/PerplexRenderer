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

		m_CurrentX = 0;
		m_CurrentY = 0;

		m_Texture.Create(size, size, TextureBufferType::LDR, TextureBufferFilterMode::Nearest);
	}

	void TextureAtlas::Destroy()
	{
		m_Texture.Destroy();
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

		// Push padded image to GPU
		glBindTexture(GL_TEXTURE_2D, m_Texture.GetID());
		glTexSubImage2D(GL_TEXTURE_2D, 0, m_CurrentX, m_CurrentY, paddedWidth, paddedHeight, GL_RGBA, GL_UNSIGNED_BYTE, paddedImage);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Cleanup
		stbi_image_free(rawImage);
		free(paddedImage);

		// Create Texture to return
		Texture subTexture;

		subTexture.PixelX = m_CurrentX + 1;
		subTexture.PixelY = m_CurrentY + 1;
		subTexture.PixelWidth = width;
		subTexture.PixelHeight = height;

		subTexture.ScaleFactorX = width / (float)m_PixelsPerUnit;
		subTexture.ScaleFactorY = height / (float)m_PixelsPerUnit;

		subTexture.Xmin = (m_CurrentX + 1) / (float)m_Size;
		subTexture.Ymin = (m_CurrentY + 1) / (float)m_Size;
		subTexture.Xmax = (m_CurrentX + width + 1) / (float)m_Size;
		subTexture.Ymax = (m_CurrentY + height + 1) / (float)m_Size;

		m_CurrentX += paddedWidth;

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
		stbi_image_free(rawImage);
		free(paddedImage);

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

	uint32_t TextureAtlas::GetAtlasTexture()
	{
		return m_Texture.GetID();
	}

	void TextureAtlas::AddPadding(int width, int height, const uint32_t* img, uint32_t* newImg)
	{
		int paddedWidth = width + 2;
		int paddedHeight = height + 2;

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
			newImg[(i / height * paddedWidth) + i % width + paddedWidth + 1] = img[i];
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
			newImg[paddedWidth + i % paddedHeight * paddedWidth] = img[i % height * width];
			newImg[2 * paddedWidth + i % paddedHeight * paddedWidth - 1] = img[width + i % height * width - 1];
		}

		// Corners
		newImg[0] = img[0];
		newImg[paddedWidth - 1] = img[width - 1];
		newImg[paddedWidth * paddedHeight - 1] = img[width * height - 1];
		newImg[paddedWidth * paddedHeight - paddedWidth] = img[width * height - width];
	}
}