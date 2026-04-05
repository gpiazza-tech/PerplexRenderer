#pragma once

#include <backends/VertexArray.h>
#include <backends/IndexBuffer.h>

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