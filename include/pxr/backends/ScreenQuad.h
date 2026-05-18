#pragma once

#include <pxr/backends/VertexArray.h>
#include <pxr/backends/IndexBuffer.h>

#include <cstdint>

namespace pxr
{
	class ScreenQuad
	{
	public:
		ScreenQuad();

		void Draw() const;
	private:
		VertexArray m_VAO;
		uint32_t m_Vao = 0;
	};
}