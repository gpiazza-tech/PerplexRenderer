#pragma once

#include "TextureAtlas.h"

#include <vector>
#include <filesystem>

namespace pxr
{
	struct Texture;

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

		void Init();

		Texture Add(const std::filesystem::path& color);
		Texture Add(const std::filesystem::path& color, const std::filesystem::path& emission);

		void BindToTextureUnits();
	private:
		// Usually only one atlas group will be needed, but if it ever runs out of room then another AtlasGroup will be added
		std::vector<AtlasGroup> m_AtlasGroups;
	};
}