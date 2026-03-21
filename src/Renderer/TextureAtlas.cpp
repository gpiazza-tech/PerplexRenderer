#include <pch.h>
#include "TextureAtlas.h"

#include "Texture.h"
#include "Util.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void TextureAtlas::Create(uint32_t size, uint32_t pixelsPerUnit)
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

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureAtlas::Destroy()
{

}

Texture TextureAtlas::AddTexture(const std::filesystem::path& path)
{
	int32_t width, height, channels;
	std::filesystem::path absolutePath = RelativePath(path);
	uint32_t* rawImage = (uint32_t*)stbi_load(absolutePath.string().c_str(), &width, &height, &channels, 4);

	// Add padding to image
	uint32_t paddedWidth = width + 2;
	uint32_t paddedHeight = height + 2;
	uint32_t* paddedImage = (uint32_t*)malloc(paddedWidth * paddedHeight * sizeof(uint32_t));

	if (!paddedImage)
	{
		std::cerr << "TextureAtlas::AddTexture: malloc failed!" << std::endl;
	}

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
		paddedImage[(i / height * paddedWidth) + i % width + paddedWidth + 1] = rawImage[i];
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
		paddedImage[i + 1] = rawImage[i];
		paddedImage[(paddedWidth * paddedHeight - 2) - i] = rawImage[(width * height - 1) - i];
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
		paddedImage[paddedWidth + i % paddedHeight * paddedWidth] = rawImage[i % height * width];
		paddedImage[2 * paddedWidth + i % paddedHeight * paddedWidth - 1] = rawImage[width + i % height * width - 1];
	}

	// Corners
	paddedImage[0] = rawImage[0];
	paddedImage[paddedWidth - 1] = rawImage[width - 1];
	paddedImage[paddedWidth * paddedHeight - 1] = rawImage[width * height - 1];
	paddedImage[paddedWidth * paddedHeight - paddedWidth] = rawImage[width * height - width];

	// Push padded image to GPU
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, m_CurrentX, m_CurrentY, paddedWidth, paddedHeight, GL_RGBA, GL_UNSIGNED_BYTE, paddedImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Cleanup
	stbi_image_free(rawImage);
	free(paddedImage);

	// Create Texture to return
	Texture subTexture;
	subTexture.AtlasTexture = m_Texture;

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

uint32_t TextureAtlas::GetAtlasTexture()
{
	return m_Texture;
}