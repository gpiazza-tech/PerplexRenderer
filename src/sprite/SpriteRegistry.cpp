#include <pxr/pch.h>
#include <pxr/sprite/SpriteRegistry.h>

#include <pxr/sprite/Sprite.h>
#include <pxr/sprite/SpriteAtlas.h>
#include <pxr/sprite/ImageBuffer.h>
#include <pxr/util/Util.h>
#include <pxr/util/Log.h>
#include <pxr/backends/RenderCommands.h>

#include <glm/fwd.hpp>

#include <vector>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

namespace pxr
{
	static std::unordered_map<std::filesystem::path, Sprite> s_Sprites;
	static Sprite s_PixelSprite;

	static std::vector<SpriteAtlas> s_Atlases;

	static glm::vec2 s_DefaultAtlasSize = { 2048, 1024 };

	void SpriteRegistry::Init(int pixelsPerUnit)
	{
		int maxTextureUnits = RenderCommands::GetMaxTextureUnits();
		int maxTextureSize = RenderCommands::GetMaxTextureSize();
		s_Atlases.reserve(maxTextureUnits);
		for (int i = 0; i < maxTextureUnits; i++)
		{
			s_Atlases.emplace_back();
			s_Atlases[i].Create((int)s_DefaultAtlasSize.x, (int)s_DefaultAtlasSize.y, pixelsPerUnit, i);
		}
		s_PixelSprite = AddSprite(LoadPNG(Path("textures/White.png")));
	}

	void SpriteRegistry::Shutdown()
	{
		for (auto& atlases : s_Atlases)
		{
			atlases.Destroy();
		}
	}

	Sprite SpriteRegistry::AddSprite(const ImageBuffer& image)
	{
		for (auto& atlas : s_Atlases)
		{
			AddSpriteResult result = atlas.AddSprite(image);

			if (result.Status == AddSpriteStatus::Success)
			{
				return result.Sprite;
			}
		}

		PXR_ASSERT(false, "Failed to create sprite from image!");
		return Sprite{};
	}

	Sprite SpriteRegistry::GetPixelSprite()
	{
		return s_PixelSprite;
	}

	std::vector<uint32_t> SpriteRegistry::GetAtlasIDs()
	{
		std::vector<uint32_t> atlasIDs(s_Atlases.size(), 0);
		for (size_t i{ 0 }; i < s_Atlases.size(); ++i)
			atlasIDs.emplace_back(s_Atlases.at(i).GetTexture()->GetID());
		return atlasIDs;
	}

	glm::ivec2 SpriteRegistry::GetAtlasSize()
	{
		return glm::ivec2{ s_Atlases.at(0).GetTexture()->GetWidth(), s_Atlases.at(0).GetTexture()->GetHeight() };
	}

	void SpriteRegistry::Bind()
	{
		int maxTextureUnits = RenderCommands::GetMaxTextureUnits();
		for (int i = 0; i < maxTextureUnits; i++)
		{
			s_Atlases.at(i).GetTexture()->BindUnit(i);
		}
	}

	void SpriteRegistry::FetchPixels(const Sprite& sprite, glm::u8vec4* pixels)
	{
		s_Atlases.at(sprite.TextureUnit).GetTexture()->GetPixels
		(sprite.PixelX, sprite.PixelY, sprite.PixelWidth, sprite.PixelHeight, pixels);
	}
}