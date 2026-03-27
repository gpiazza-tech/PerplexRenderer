# PerplexRenderer

PerplexRenderer is a neo pixel art 2D renderer for Windows using OpenGL. I plan on adding support soon for Linux and eventually MacOS with Metal. 

![Rendered Logo](https://github.com/gpiazza-tech/PerplexRenderer/blob/master/res/screenshots/Logo.png)

## How to build

Windows
 * Ensure git and Visual Studio are installed
 * Navigate to your desired installation directory and run `git clone https://github.com/gpiazza-tech/PerplexRenderer`
 * Run scripts/Setup-Windows.bat to build as a static library or scripts/Setup-Examples-Windows.bat to build an example application. (Note: building as a static library will not link ImGui or GLFW, only Glew will be linked)
 * Open the generated .sln file in Visual Studio, and build the solution
