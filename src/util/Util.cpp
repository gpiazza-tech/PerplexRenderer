#include <pch.h>
#include "Util.h"

#include <fwd.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace pxr
{
    std::string g_ProjectPath = PXR_ROOT_DIR;

    std::string StringFromFile(const std::filesystem::path& path)
    {
        std::ifstream file(path.string());
        std::stringstream buffer;

        buffer << file.rdbuf();
        return buffer.str();
    }

    std::filesystem::path RelativePath(const std::filesystem::path& path)
    {
        return g_ProjectPath / path;
    }

    float RoundToNearestFraction(float val, float denominator)
    {
        // if denominator (d) = 16: val * 16, round(val), val / 16
        double buffer = val * denominator;
        buffer = round(buffer);
        return buffer / denominator;
    }

    glm::vec3 MakePixelPerfect(glm::vec3 vec3, int pixelsPerUnit)
    {
        return glm::vec3(RoundToNearestFraction(vec3.x, pixelsPerUnit), RoundToNearestFraction(vec3.y, pixelsPerUnit), RoundToNearestFraction(vec3.z, pixelsPerUnit));
    }
}