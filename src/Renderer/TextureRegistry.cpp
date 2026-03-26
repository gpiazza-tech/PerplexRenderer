#include <pch.h>
#include "TextureRegistry.h"

#include "Texture.h"

#include <vector>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

TextureRegistry::TextureRegistry()
: m_AtlasGroups(std::vector<AtlasGroup>(1))
{
}

void TextureRegistry::Init()
{
	m_AtlasGroups[0].ColorAtlas.Create(1024, 16);
	m_AtlasGroups[0].EmissionAtlas.Create(1024, 16);
}

Texture TextureRegistry::Add(const fs::path& color)
{
	Texture colorTexture = m_AtlasGroups[0].ColorAtlas.AddTexture(color);
	colorTexture.ColorAtlas = m_AtlasGroups[0].ColorAtlas.GetAtlasTexture();
	colorTexture.EmissionAtlas = m_AtlasGroups[0].EmissionAtlas.GetAtlasTexture();

	return colorTexture;
}

Texture TextureRegistry::Add(const fs::path& color, const fs::path& emission)
{ 
	Texture colorTexture = m_AtlasGroups[0].ColorAtlas.AddTexture(color);
	colorTexture.ColorAtlas = m_AtlasGroups[0].ColorAtlas.GetAtlasTexture();
	colorTexture.EmissionAtlas = m_AtlasGroups[0].EmissionAtlas.GetAtlasTexture();

	Texture emissionTexture = m_AtlasGroups[0].EmissionAtlas.AddTextureAt(emission, colorTexture.PixelX, colorTexture.PixelY);
	emissionTexture.ColorAtlas = m_AtlasGroups[0].ColorAtlas.GetAtlasTexture();
	emissionTexture.EmissionAtlas = m_AtlasGroups[0].EmissionAtlas.GetAtlasTexture();

	if (colorTexture.PixelWidth != emissionTexture.PixelWidth || colorTexture.PixelHeight != emissionTexture.PixelHeight)
	{
		// TODO: Validate the emission texture size BEFORE adding it to emission atlas
		std::cout << "Emission texture size does not match color texture size!" << "\n"
			<< "Color: " << color << " | Emission: " << emission;
	}

	return colorTexture;
}

void TextureRegistry::BindToTextureUnits()
{
	int i = 0;
	for (size_t i = 0; i < m_AtlasGroups.size(); i++)
	{
		glBindTextureUnit(i + 0, m_AtlasGroups[i].ColorAtlas.GetAtlasTexture());
		glBindTextureUnit(i + 1, m_AtlasGroups[i].EmissionAtlas.GetAtlasTexture());

		i += 2;
	}
}