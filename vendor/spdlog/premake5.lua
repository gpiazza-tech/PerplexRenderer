project "spdlog"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    staticruntime "on"
	toolset "msc-v145"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files "src/*.cpp"
	includedirs "include" 

	defines "SPDLOG_COMPILED_LIB"
	buildoptions "/utf-8"

	filter "system:windows"
        systemversion "latest"
		
	filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"