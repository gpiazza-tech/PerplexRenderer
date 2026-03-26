#pragma once

#include <fwd.hpp>

#include <filesystem>
#include <string>

namespace pxr
{
	std::string StringFromFile(const std::filesystem::path& path);
	std::filesystem::path RelativePath(const std::filesystem::path& path);

	float RoundToNearestFraction(float val, float denominator);
	glm::vec3 MakePixelPerfect(glm::vec3 vec3, int pixelsPerUnit);
}