#pragma once

#include "Sprite.h"

#include <fwd.hpp>

#include <filesystem>

namespace pxr
{
	class Renderer;

	class SpriteRegistry
	{
	public:
		static void Init(int pixelsPerUnit);
		static void Shutdown();

		static const Sprite& GetSprite(const std::filesystem::path& spritePath);
		static const Sprite& GetPixelSprite();

		static void Bind();

		static void FetchPixels(const Sprite& sprite, glm::u8vec4* pixels);
	};
}