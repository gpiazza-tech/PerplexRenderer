#include <pch.h>
#include "SpriteRegistry.h"

#include "Sprite.h"
#include <backends/RenderCommands.h>
#include <util/Log.h>

#include <vector>
#include <unordered_map>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

namespace pxr
{
	static std::unordered_map<std::filesystem::path, Sprite> s_Sprites;
	static Sprite s_PixelSprite;

	static std::vector<SpriteAtlas> s_Atlases;

	static glm::vec2 s_DefaultAtlasSize = { 1024, 1024 };

	void SpriteRegistry::Init(int pixelsPerUnit)
	{
		int maxTextureUnits = RenderCommands::GetMaxTextureUnits();
		s_Atlases.reserve(maxTextureUnits);
		for (int i = 0; i < maxTextureUnits; i++)
		{
			s_Atlases.emplace_back();
			s_Atlases[i].Create(s_DefaultAtlasSize.x, s_DefaultAtlasSize.y, pixelsPerUnit, i);
		}

		s_PixelSprite = GetSprite("res\\textures\\White.png");
	}

	void SpriteRegistry::Shutdown()
	{
		for (auto& atlases : s_Atlases)
		{
			atlases.Destroy();
		}
	}

	const Sprite& SpriteRegistry::GetSprite(const std::filesystem::path& spritePath)
	{
		if (s_Sprites.find(spritePath) != s_Sprites.end())
		{
			return s_Sprites[spritePath];
		}
		else
		{
			for (auto& atlas : s_Atlases)
			{
				const AddSpriteResult& result = atlas.AddSprite(spritePath);
				if (result.Status == AddSpriteStatus::Success)
				{
					PXR_INFO("Creating sprite from path {0}", spritePath.string());

					s_Sprites[spritePath] = result.Sprite;
					return s_Sprites[spritePath];
				}
			}

			PXR_ERROR("Failed to add sprite to any atlas in SpriteRegistry!");
		}
	}

	const Sprite& SpriteRegistry::GetPixelSprite()
	{
		return GetSprite("res\\textures\\White.png");
	}

	void SpriteRegistry::Bind()
	{
		int maxTextureUnits = RenderCommands::GetMaxTextureUnits();
		for (int i = 0; i < maxTextureUnits; i++)
		{
			s_Atlases[i].GetTexture()->BindUnit(i);
		}
	}

	void SpriteRegistry::FetchPixels(const Sprite& sprite, glm::u8vec4* pixels)
	{
		s_Atlases[sprite.TextureUnit].GetTexture()->GetPixels
		(sprite.PixelX, sprite.PixelY, sprite.PixelWidth, sprite.PixelHeight, pixels);
	}
}