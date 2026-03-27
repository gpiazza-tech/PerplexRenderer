workspace "PerplexRenderer"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    startproject "PerplexRenderer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["stb_image"] = "vendor/stb_image/include"
IncludeDir["glm"] = "vendor/glm/include"
IncludeDir["imgui"] = "vendor/imgui/include"
IncludeDir["glfw"] = "vendor/glfw/include"
IncludeDir["glew"] = "vendor/glew/include"

group "Dependencies"
    include "vendor/imgui"
group ""

project "PerplexRenderer"
    location "."
    kind "ConsoleApp"
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
        "examples/**.h",
        "examples/**.cpp",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp",
        "vendor/imgui/include/backends/imgui_impl_glfw.cpp",
        "vendor/imgui/include/backends/imgui_impl_opengl3.cpp"
    }

    includedirs
    {
        "src",
        "include",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glew}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
    }

    libdirs
    {
        "vendor/glfw/lib",
        "vendor/glew/lib",
        "vendor/imgui/bin",
    }

    defines
    {
        "GLEW_STATIC",
        "PXR_EXAMPLES",
        'PXR_ROOT_DIR="' .. path.getabsolute(".") .. '"'
    }

    links
    {
        "ImGui.lib",
        "glfw3_mt.lib",
        "glew32s.lib",
        "opengl32.lib"
    }

    buildoptions
    {
        "/utf-8"
    }

    filter "files:vendor/imgui/include/backends/**.cpp"
        flags {"NoPCH"}

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "PXR_PLATFORM_WINDOWS"
        }
    
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