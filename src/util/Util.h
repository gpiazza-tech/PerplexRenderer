#pragma once

#include <glm/fwd.hpp>

#include <filesystem>
#include <string>

namespace pxr
{
	std::string StringFromFile(const std::filesystem::path& path);
	std::filesystem::path Path(const std::filesystem::path& path);
	void SetResourceFolder(const std::filesystem::path& path);

	float RoundToNearestFraction(float val, float denominator);
	glm::vec3 MakePixelPerfect(glm::vec3 vec3, int pixelsPerUnit);

	void* ImageLoad(const std::filesystem::path& path, int* width, int* height, int* channels, int desiredChannels);
	void ImageFree(void* imageData);
}