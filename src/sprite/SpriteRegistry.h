#pragma once

#include "SpriteAtlas.h"
#include "Sprite.h"

#include <vector>
#include <filesystem>

namespace pxr
{
	class Renderer;

	struct AtlasGroup
	{
		SpriteAtlas ColorAtlas;
		SpriteAtlas EmissionAtlas;
	};

	class SpriteRegistry
	{
	public:
		SpriteRegistry();
		~SpriteRegistry() { }

		void Create(int size, int pixelsPerUnit);
		void Destroy();

		Sprite Add(const std::filesystem::path& color);
		Sprite Add(const std::filesystem::path& color, const std::filesystem::path& emission);

		void Bind();

		std::vector<AtlasGroup>& GetAtlasGroups() { return m_AtlasGroups; }
		const Sprite& GetPixelSprite() const { return m_PixelSprite; };

		glm::u8vec4* FetchColorPixels(const Sprite& sprite);
		glm::u8vec4* FetchEmissionPixels(const Sprite& sprite);
	private:
		// Usually only one atlas group will be needed, but if it ever runs out of room then another AtlasGroup will be added
		std::vector<AtlasGroup> m_AtlasGroups;
		Sprite m_PixelSprite;
	};
}