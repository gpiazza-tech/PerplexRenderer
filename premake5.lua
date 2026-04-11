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
IncludeDir["spdlog"] = "vendor/spdlog/include"
IncludeDir["glm"] = "vendor/glm/include"
IncludeDir["glew"] = "vendor/glew/include"

group "Dependencies"
    include "vendor/spdlog"
group ""


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
        "%{IncludeDir.spdlog}",
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
    }

    links
    {
        "spdlog",
        "glew32s.lib",
        "opengl32.lib"
    }

    buildoptions
    {
        "/utf-8"
    }

    postbuildcommands 
    {
      -- copy res folder into output directory
      "{COPYDIR} %{path.getabsolute('res')} %{cfg.targetdir}/res"
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