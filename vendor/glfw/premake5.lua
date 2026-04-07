project "glfw"
	kind "StaticLib"
	language "C"
    staticruntime "on"
	toolset "msc-v145"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/context.c", 
		"src/init.c", 
		"src/input.c", 
		"src/monitor.c",
        "src/platform.c",
		"src/vulkan.c",
		"src/window.c",
		"src/null_init.c",
		"src/null_monitor.c",
		"src/null_window.c",
		"src/null_joystick.c",
	}

	includedirs
	{
		"include",
		"include/GLFW"
	}

	filter "system:windows"
        systemversion "latest"
		links "OpenGL32"
		defines { "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" }

		files
		{
			"include/**.h",
        	"src/win32_init.c", 
			"src/win32_joystick.c", 
			"src/win32_monitor.c",
        	"src/win32_time.c", 
			"src/win32_thread.c", 
			"src/win32_window.c",
			"src/win32_module.c",
        	"src/wgl_context.c",
			"src/egl_context.c",
			"src/osmesa_context.c",
		}
		
	filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"