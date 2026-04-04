#pragma once

namespace pxr
{
	class Shader;

	class RenderCommands
	{
	public:
		static void DrawScreen();
		static void ResizeViewport(int width, int height);
	};
}