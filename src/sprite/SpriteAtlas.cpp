#include <pxr/pch.h>
#include <pxr/sprite/SpriteAtlas.h>

#include <pxr/sprite/Sprite.h>
#include <pxr/sprite/ImageBuffer.h>
#include <pxr/backends/TextureBuffer.h>
#include <pxr/backends/RenderCommands.h>
#include <pxr/util/Util.h>
#include <pxr/util/Log.h>

#include <cstdint>
#include <filesystem>
#include <glm/fwd.hpp>
#include <malloc.h>

namespace pxr
{
	void SpriteAtlas::Create(int width, int height, int pixelsPerUnit, int textureUnit)
	{
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

	AddSpriteResult SpriteAtlas::AddSprite(const ImageBuffer& imageBuffer)
	{
		AddSpriteResult result{};

		ImageBuffer paddedImage = imageBuffer;
		AddPadding(paddedImage);

		size_t imageWidth = paddedImage.GetWidth();
		size_t imageHeight = paddedImage.GetHeight();

		// determine position
		int shelfIndex = GetShelfIndex(static_cast<int>(imageHeight));
		for (int i = (int)m_Shelves.size(); i <= shelfIndex; i++)
			// start at the shelf size and continue adding until we reach the desired index
		{
			m_Shelves.emplace_back(Shelf(-1, m_PixelsPerUnit * i + m_PixelsPerUnit + 2, 0));
		}
		Shelf& shelf = m_Shelves[shelfIndex];
		if (shelf.Y == -1 || shelf.NextTextureX + imageWidth >= m_Width)
			// if shelf does not exist or runs out of room
		{
			shelf = Shelf(m_NextShelf, shelf.Height, 0);
			m_NextShelf += shelf.Height + 1;
		}
		int x = shelf.NextTextureX;
		int y = shelf.Y;
		shelf.NextTextureX += static_cast<int>(imageWidth);

		if (m_NextShelf > m_MaxSize)
			// Completely out of room, return AddSpriteStatus::Fail
		{
			result.Status = AddSpriteStatus::Fail;
			return result;
		}

		Sprite subTexture = AllocateBuffer(x, y, imageWidth, imageHeight, (uint32_t*)paddedImage.Data());

		result.Sprite = subTexture;
		result.Status = AddSpriteStatus::Success;

		return result;
	}

	void SpriteAtlas::AddPadding(ImageBuffer& image)
	{
		size_t oldWidth{ image.GetWidth() }, oldHeight{ image.GetHeight() };
		
		// Add padding
		image.Resize(1, 1, 1, 1);

		size_t paddedWidth{ image.GetWidth() }, paddedHeight{ image.GetHeight() };

		// Set vertical padding:
		for (size_t x{}; x < oldWidth; ++x)
		{
			image.At(x, 0) = image.At(x, 1);
			image.At(x, paddedHeight - 1) = image.At(x, oldHeight - 1);
		}

		// Set horizontal padding
		for (size_t y{}; y < oldHeight; ++y)
		{
			image.At(0, y) = image.At(1, y);
			image.At(paddedWidth - 1, y) = image.At(oldWidth - 1, y);
		}

		// Corners
		image.At(0, 0) = image.At(1, 1);
		image.At(paddedWidth - 1, 0) = image.At(oldWidth - 1, 1);
		image.At(paddedWidth - 1, paddedHeight - 1) = image.At(oldWidth - 1, oldHeight - 1);
		image.At(0, paddedHeight - 1) = image.At(1, oldHeight - 1);
	}

	int SpriteAtlas::GetShelfIndex(int textureHeight) const
	{
		// If p equals m_PixelsPerUnit and i is the index of the vector, then m_Shelves[i] holds the y position for the shelf with a range of (pi+3) to p(i+1)+2
		// Figuring out which index should be returned based on the height is as simple as solving for i for the min value of the range:
		// minHeight = pi+3 -> i = (minHeight - 3) / p
		// integer division will handle the rest:
		 
		return (textureHeight - 3) / m_PixelsPerUnit;
	}

	Sprite SpriteAtlas::AllocateBuffer(size_t x, size_t y, size_t width, size_t height, uint32_t* bytes)
	{
		PXR_ASSERT(width > 2 && height > 2, "Buffer width and height are too small to allocate!");

		int ix = static_cast<int>(x);
		int iy = static_cast<int>(y);
		int iwidth = static_cast<int>(width);
		int iheight = static_cast<int>(height);

		// Push padded image to GPU
		m_Texture->SetPixels(ix, iy, iwidth, iheight, (glm::u8vec4*)bytes);

		// Create Sprite to return
		Sprite sprite;

		size_t visibleWidth = width - 2;
		size_t visibleHeight = height - 2;

		sprite.PixelX = ix + 1;
		sprite.PixelY = iy + 1;
		sprite.PixelWidth = static_cast<int>(visibleWidth);
		sprite.PixelHeight = static_cast<int>(visibleHeight);

		sprite.ScaleFactorX = visibleWidth / (float)m_PixelsPerUnit;
		sprite.ScaleFactorY = visibleHeight / (float)m_PixelsPerUnit;

		sprite.Xmin = (ix + 1) / (float)m_Width;
		sprite.Ymin = (iy + 1) / (float)m_Height;
		sprite.Xmax = (x + 1 + visibleWidth) / (float)m_Width;
		sprite.Ymax = (y + 1 + visibleHeight) / (float)m_Height;

		sprite.TextureUnit = m_TextureUnit;

		return sprite;
	}
}