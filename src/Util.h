#pragma once

#include <string>
#include <filesystem>
#include <fwd.hpp>

std::string StringFromFile(const std::filesystem::path& path);
std::filesystem::path RelativePath(const std::filesystem::path& path);

float RoundToNearestFraction(float val, float denominator);
glm::vec3 MakePixelPerfect(glm::vec3 vec3, int pixelsPerUnit);