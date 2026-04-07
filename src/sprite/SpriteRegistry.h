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
		static void Init(int pixelsPerUnit);
		static void Shutdown();

		static Sprite Add(const std::filesystem::path& color);
		static Sprite Add(const std::filesystem::path& color, const std::filesystem::path& emission);

		static void Bind();

		static std::vector<AtlasGroup>& GetAtlasGroups();
		static const Sprite& GetPixelSprite();

		static glm::u8vec4* FetchColorPixels(const Sprite& sprite);
		static glm::u8vec4* FetchEmissionPixels(const Sprite& sprite);
	};
}