#include <pch.h>
#include "TextureRegistry.h"

#include "Texture.h"
#include <rendering/Renderer.h>

#include <vector>
#include <filesystem>
#include <iostream>
#include <cstdint>

namespace fs = std::filesystem;

namespace pxr
{
	TextureRegistry::TextureRegistry()
		: m_AtlasGroups(std::vector<AtlasGroup>(1))
	{
	}

	void TextureRegistry::Create(int size, int pixelsPerUnit)
	{
		m_AtlasGroups[0].ColorAtlas.Create(size, pixelsPerUnit);
		m_AtlasGroups[0].EmissionAtlas.Create(size, pixelsPerUnit);

		m_WhiteTexture = Add("res\\textures\\White.png", "res\\textures\\White.png");
	}

	void TextureRegistry::Destroy()
	{
		m_AtlasGroups[0].ColorAtlas.Destroy();
		m_AtlasGroups[0].EmissionAtlas.Destroy();
	}

	Texture TextureRegistry::Add(const fs::path& color)
	{
		Texture colorTexture = m_AtlasGroups[0].ColorAtlas.AddTexture(color);
		colorTexture.ColorAtlas = m_AtlasGroups[0].ColorAtlas.GetTexture().GetID();
		colorTexture.EmissionAtlas = m_AtlasGroups[0].EmissionAtlas.GetTexture().GetID();

		colorTexture.TextureRegistry = this;
		return colorTexture;
	}

	Texture TextureRegistry::Add(const fs::path& color, const fs::path& emission)
	{
		Texture colorTexture = m_AtlasGroups[0].ColorAtlas.AddTexture(color);
		colorTexture.ColorAtlas = m_AtlasGroups[0].ColorAtlas.GetTexture().GetID();
		colorTexture.EmissionAtlas = m_AtlasGroups[0].EmissionAtlas.GetTexture().GetID();

		Texture emissionTexture = m_AtlasGroups[0].EmissionAtlas.AddTextureAt(emission, colorTexture.PixelX, colorTexture.PixelY);
		emissionTexture.ColorAtlas = m_AtlasGroups[0].ColorAtlas.GetTexture().GetID();
		emissionTexture.EmissionAtlas = m_AtlasGroups[0].EmissionAtlas.GetTexture().GetID();

		if (colorTexture.PixelWidth != emissionTexture.PixelWidth || colorTexture.PixelHeight != emissionTexture.PixelHeight)
		{
			// TODO: Validate the emission texture size BEFORE adding it to emission atlas
			std::cout << "Emission texture size does not match color texture size!" << "\n"
				<< "Color: " << color << " | Emission: " << emission;
		}

		colorTexture.TextureRegistry = this;
		return colorTexture;
	}

	void TextureRegistry::Bind()
	{
		for (size_t atlasIndex = 0; atlasIndex < m_AtlasGroups.size(); atlasIndex++)
		{
			uint32_t baseUnit = (uint32_t)atlasIndex * 2;

			m_AtlasGroups[atlasIndex].ColorAtlas.GetTexture().BindUnit(baseUnit);
			m_AtlasGroups[atlasIndex].EmissionAtlas.GetTexture().BindUnit(baseUnit + 1);
		}

		Renderer::SetPixelSprite(m_WhiteTexture);
	}

	glm::u8vec4* TextureRegistry::FetchColorPixels(const Texture& texture)
	{
		return m_AtlasGroups[0].ColorAtlas.GetTexture().FetchPixels
			(texture.PixelX, texture.PixelY, texture.PixelWidth, texture.PixelHeight);
	}

	glm::u8vec4* TextureRegistry::FetchEmissionPixels(const Texture& texture)
	{
		return m_AtlasGroups[0].EmissionAtlas.GetTexture().FetchPixels
			(texture.PixelX, texture.PixelY, texture.PixelWidth, texture.PixelHeight);
	}
}