#pragma once

#include "TextureAtlas.h"
#include "Texture.h"

#include <vector>
#include <filesystem>

namespace pxr
{
	class Renderer;

	struct AtlasGroup
	{
		TextureAtlas ColorAtlas;
		TextureAtlas EmissionAtlas;
	};

	class TextureRegistry
	{
	public:
		TextureRegistry();
		~TextureRegistry() { }

		void Create(int size, int pixelsPerUnit);
		void Destroy();

		Texture Add(const std::filesystem::path& color);
		Texture Add(const std::filesystem::path& color, const std::filesystem::path& emission);

		void Bind();

		std::vector<AtlasGroup>& GetAtlasGroups() { return m_AtlasGroups; }
		const Texture& GetWhiteTexture() const { return m_WhiteTexture; };

		glm::u8vec4* FetchColorPixels(const Texture& texture);
		glm::u8vec4* FetchEmissionPixels(const Texture& texture);
	private:
		// Usually only one atlas group will be needed, but if it ever runs out of room then another AtlasGroup will be added
		std::vector<AtlasGroup> m_AtlasGroups;
		Texture m_WhiteTexture;
	};
}