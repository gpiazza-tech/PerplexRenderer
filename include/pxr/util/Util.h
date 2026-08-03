#pragma once

#include <pxr/sprite/ImageBuffer.h>

#include <glm/fwd.hpp>

#include <filesystem>
#include <string>

namespace pxr
{
	std::string StringFromFile(const std::filesystem::path& path);
	std::filesystem::path Path(const std::filesystem::path& path);
	void SetResourceFolder(const std::filesystem::path& path);

	float RoundToNearestFraction(float val, float denominator);
	float MakePixelPerfect(float val, int pixelsPerUnit);
	glm::vec2 MakePixelPerfect(glm::vec2 vec2, int pixelsPerUnit);
	glm::vec3 MakePixelPerfect(glm::vec3 vec3, int pixelsPerUnit);

	ImageBuffer LoadPNG(const std::filesystem::path& path);
	void SavePNG(const std::filesystem::path& path, const ImageBuffer& imageBuffer);

	void* ImageLoad(const std::filesystem::path& path, int* width, int* height, int* channels, int desiredChannels);
	void ImageFree(void* imageData);
}