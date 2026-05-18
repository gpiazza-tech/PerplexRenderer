#pragma once

#include <cstdint>

namespace pxr
{
	class VertexBuffer;
	class IndexBuffer;

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void AttachBuffers(const VertexBuffer& vbo, const IndexBuffer& ibo);

		void Bind() const;
		void Unbind() const;
	private:
		uint32_t m_RendererID = 0;
	};
}