#pragma once

#include <cstdint>

struct Texture
{
	uint32_t AtlasTexture;

	uint32_t PixelX;
	uint32_t PixelY;
	uint32_t PixelWidth;
	uint32_t PixelHeight;

	float ScaleFactorX;
	float ScaleFactorY;

	float Xmin;
	float Ymin;
	float Xmax;
	float Ymax;
};