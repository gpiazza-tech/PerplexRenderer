project "glew"
	kind "StaticLib"
	language "C"
    staticruntime "on"
	toolset "msc-v145"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{ 
        "include/GL/*.h", 
        "src/glew.c"
    }

	includedirs
	{
		"include"
	}

	defines { "GLEW_STATIC" }

	filter "system:windows"
        systemversion "latest"

		defines { "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS" }
        links { "opengl32" }
		
	filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"