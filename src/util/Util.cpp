#include <pch.h>
#include "Util.h"

#include "Log.h"

#include <fwd.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace pxr
{
    static std::filesystem::path s_ResourcePath = std::filesystem::current_path() / "res";

    std::string StringFromFile(const std::filesystem::path& path)
    {
        std::ifstream file(path.string());
        std::stringstream buffer;

        buffer << file.rdbuf();
        return buffer.str();
    }

    std::filesystem::path Path(const std::filesystem::path& path)
    {
        std::filesystem::path absolutePath = s_ResourcePath / path;
        PXR_ASSERT(std::filesystem::exists(absolutePath), "Path {1} does not exist!", absolutePath.string());

        return absolutePath;
    }

    void SetResourceFolder(const std::filesystem::path& path)
    {
        s_ResourcePath = path;
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