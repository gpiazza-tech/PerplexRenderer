workspace "PerplexRenderer"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["stb_image"] = "vendor/stb_image/include"
IncludeDir["glm"] = "vendor/glm/include"
IncludeDir["glew"] = "vendor/glew/include"

project "PerplexRenderer"
    location "."
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"
    toolset "msc-v145"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp",
    }

    includedirs
    {
        "src",
        "include",
        "%{IncludeDir.glew}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
    }

    libdirs
    {
        "vendor/glew/lib",
    }

    defines
    {
        "GLEW_STATIC",
        'PXR_ROOT_DIR="' .. path.getabsolute(".") .. '"'
    }

    links
    {
        "glew32s.lib",
        "opengl32.lib"
    }

    buildoptions
    {
        "/utf-8"
    }

    filter "system:windows"
        systemversion "latest"
        defines "PXR_PLATFORM_WINDOWS"
    
    filter "configurations:Debug"
        defines "PXR_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "PXR_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "PXR_DIST"
        runtime "Release"
        optimize "on"