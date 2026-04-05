#pragma once

#include <fwd.hpp>

#include <cstdint>

namespace pxr
{
	class Shader;
	class VertexArray;

	class RenderCommands
	{
	public:
		static void DrawScreen();
		static void ResizeViewport(int width, int height);

		static void DrawTriangles(const VertexArray& VAO, uint32_t count);

		static void EnableAlphaBlending();

		static void Clear(const glm::vec4& color);
		static void ClearColor(const glm::vec4& color);
		static void ClearDepth();
		static void ClearStencil();

		static void EnableDepthTest();
		static void DisableDepthTest();
	};
}