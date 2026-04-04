#pragma once

#include <cstdint>

namespace pxr
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;
	private:
		uint32_t m_RendererID = 0;
	};
}