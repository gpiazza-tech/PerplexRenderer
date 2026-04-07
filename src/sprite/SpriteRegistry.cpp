#include <pch.h>
#include "SpriteRegistry.h"

#include "Sprite.h"
#include <util/Log.h>

#include <vector>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

namespace pxr
{
	static std::vector<AtlasGroup> s_AtlasGroups;
	static Sprite s_PixelSprite;

	static int s_AtlasSize = 1024;

	void SpriteRegistry::Init(int pixelsPerUnit)
	{
		s_AtlasGroups.emplace_back();

		s_AtlasGroups[0].ColorAtlas.Create(s_AtlasSize, pixelsPerUnit);
		s_AtlasGroups[0].EmissionAtlas.Create(s_AtlasSize, pixelsPerUnit);

		s_PixelSprite = Add("res\\textures\\White.png", "res\\textures\\White.png");
	}

	void SpriteRegistry::Shutdown()
	{
		for (auto& atlasGroup : s_AtlasGroups)
		{
			atlasGroup.ColorAtlas.Destroy();
			atlasGroup.EmissionAtlas.Destroy();
		}
	}

	Sprite SpriteRegistry::Add(const std::filesystem::path& color)
	{
		Sprite colorSprite = s_AtlasGroups[0].ColorAtlas.AddTexture(color);
		colorSprite.ColorAtlas = s_AtlasGroups[0].ColorAtlas.GetTexture()->GetID();
		colorSprite.EmissionAtlas = s_AtlasGroups[0].EmissionAtlas.GetTexture()->GetID();

		return colorSprite;
	}

	Sprite SpriteRegistry::Add(const std::filesystem::path& color, const std::filesystem::path& emission)
	{
		Sprite colorSprite = s_AtlasGroups[0].ColorAtlas.AddTexture(color);
		colorSprite.ColorAtlas = s_AtlasGroups[0].ColorAtlas.GetTexture()->GetID();
		colorSprite.EmissionAtlas = s_AtlasGroups[0].EmissionAtlas.GetTexture()->GetID();

		Sprite emissionSprite = s_AtlasGroups[0].EmissionAtlas.AddTextureAt(emission, colorSprite.PixelX, colorSprite.PixelY);
		emissionSprite.ColorAtlas = s_AtlasGroups[0].ColorAtlas.GetTexture()->GetID();
		emissionSprite.EmissionAtlas = s_AtlasGroups[0].EmissionAtlas.GetTexture()->GetID();

		PXR_ASSERT(colorSprite.PixelWidth == emissionSprite.PixelWidth && colorSprite.PixelHeight == emissionSprite.PixelHeight,
			"Emission sprite size does not match color sprite size!\nColor: {1}\nEmission: {2}", color.string(), emission.string());

		return colorSprite;
	}

	void SpriteRegistry::Bind()
	{
		for (size_t atlasIndex = 0; atlasIndex < s_AtlasGroups.size(); atlasIndex++)
		{
			uint32_t baseUnit = (uint32_t)atlasIndex * 2;

			s_AtlasGroups[atlasIndex].ColorAtlas.GetTexture()->BindUnit(baseUnit);
			s_AtlasGroups[atlasIndex].EmissionAtlas.GetTexture()->BindUnit(baseUnit + 1);
		}
	}

	std::vector<AtlasGroup>& SpriteRegistry::GetAtlasGroups()
	{
		return s_AtlasGroups;
	}

	const Sprite& SpriteRegistry::GetPixelSprite()
	{
		return s_PixelSprite;
	}

	glm::u8vec4* SpriteRegistry::FetchColorPixels(const Sprite& sprite)
	{
		return s_AtlasGroups[0].ColorAtlas.GetTexture()->FetchPixels
		(sprite.PixelX, sprite.PixelY, sprite.PixelWidth, sprite.PixelHeight);
	}

	glm::u8vec4* SpriteRegistry::FetchEmissionPixels(const Sprite& sprite)
	{
		return s_AtlasGroups[0].EmissionAtlas.GetTexture()->FetchPixels
		(sprite.PixelX, sprite.PixelY, sprite.PixelWidth, sprite.PixelHeight);
	}
}