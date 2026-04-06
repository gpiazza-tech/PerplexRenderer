project "glfw"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    staticruntime "on"
	toolset "msc-v145"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/*.c",
		"src/*.h",
	}

	includedirs
	{
		"include"
	}

	filter "system:windows"
        systemversion "latest"
		
	filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"