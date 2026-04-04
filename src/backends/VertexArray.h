#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace pxr
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

		void Bind() const;
		void Unbind() const;
	private:
		uint32_t m_RendererID = 0;
	};
}