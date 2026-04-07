#include <pch.h>
#include "SpriteRegistry.h"

#include "Sprite.h"
#include <rendering/Renderer.h>

#include <vector>
#include <filesystem>
#include <iostream>
#include <cstdint>

namespace fs = std::filesystem;

namespace pxr
{
	SpriteRegistry::SpriteRegistry()
		: m_AtlasGroups(std::vector<AtlasGroup>(1))
	{
	}

	void SpriteRegistry::Create(int size, int pixelsPerUnit)
	{
		m_AtlasGroups[0].ColorAtlas.Create(size, pixelsPerUnit);
		m_AtlasGroups[0].EmissionAtlas.Create(size, pixelsPerUnit);

		m_PixelSprite = Add("res\\textures\\White.png", "res\\textures\\White.png");
	}

	void SpriteRegistry::Destroy()
	{
		m_AtlasGroups[0].ColorAtlas.Destroy();
		m_AtlasGroups[0].EmissionAtlas.Destroy();
	}

	Sprite SpriteRegistry::Add(const fs::path& color)
	{
		Sprite colorSprite = m_AtlasGroups[0].ColorAtlas.AddTexture(color);
		colorSprite.ColorAtlas = m_AtlasGroups[0].ColorAtlas.GetTexture()->GetID();
		colorSprite.EmissionAtlas = m_AtlasGroups[0].EmissionAtlas.GetTexture()->GetID();

		colorSprite.SpriteRegistry = this;
		return colorSprite;
	}

	Sprite SpriteRegistry::Add(const fs::path& color, const fs::path& emission)
	{
		Sprite colorSprite = m_AtlasGroups[0].ColorAtlas.AddTexture(color);
		colorSprite.ColorAtlas = m_AtlasGroups[0].ColorAtlas.GetTexture()->GetID();
		colorSprite.EmissionAtlas = m_AtlasGroups[0].EmissionAtlas.GetTexture()->GetID();

		Sprite emissionSprite = m_AtlasGroups[0].EmissionAtlas.AddTextureAt(emission, colorSprite.PixelX, colorSprite.PixelY);
		emissionSprite.ColorAtlas = m_AtlasGroups[0].ColorAtlas.GetTexture()->GetID();
		emissionSprite.EmissionAtlas = m_AtlasGroups[0].EmissionAtlas.GetTexture()->GetID();

		if (colorSprite.PixelWidth != emissionSprite.PixelWidth || colorSprite.PixelHeight != emissionSprite.PixelHeight)
		{
			// TODO: Validate the emission texture size BEFORE adding it to emission atlas
			std::cout << "Emission sprite size does not match color sprite size!" << "\n"
				<< "Color: " << color << " | Emission: " << emission;
		}

		colorSprite.SpriteRegistry = this;
		return colorSprite;
	}

	void SpriteRegistry::Bind()
	{
		for (size_t atlasIndex = 0; atlasIndex < m_AtlasGroups.size(); atlasIndex++)
		{
			uint32_t baseUnit = (uint32_t)atlasIndex * 2;

			m_AtlasGroups[atlasIndex].ColorAtlas.GetTexture()->BindUnit(baseUnit);
			m_AtlasGroups[atlasIndex].EmissionAtlas.GetTexture()->BindUnit(baseUnit + 1);
		}

		Renderer::SetPixelSprite(m_PixelSprite);
	}

	glm::u8vec4* SpriteRegistry::FetchColorPixels(const Sprite& sprite)
	{
		return m_AtlasGroups[0].ColorAtlas.GetTexture()->FetchPixels
			(sprite.PixelX, sprite.PixelY, sprite.PixelWidth, sprite.PixelHeight);
	}

	glm::u8vec4* SpriteRegistry::FetchEmissionPixels(const Sprite& sprite)
	{
		return m_AtlasGroups[0].EmissionAtlas.GetTexture()->FetchPixels
			(sprite.PixelX, sprite.PixelY, sprite.PixelWidth, sprite.PixelHeight);
	}
}