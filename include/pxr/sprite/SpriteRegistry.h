#pragma once

#include <pxr/sprite/Sprite.h>
#include <pxr/sprite/ImageBuffer.h>

#include <glm/fwd.hpp>

#include <cstdint>
#include <vector>

namespace pxr
{
	class Renderer;

	class SpriteRegistry
	{
	public:
		static void Init(int pixelsPerUnit);
		static void Shutdown();

		static Sprite AddSprite(const ImageBuffer& image);
		static Sprite GetPixelSprite();

		static std::vector<uint32_t> GetAtlasIDs();
		static glm::ivec2 GetAtlasSize();

		static void Bind();

		static void FetchPixels(const Sprite& sprite, glm::u8vec4* pixels);
	};
}