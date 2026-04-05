#pragma once

#include <util/Type.h>

#include <cstdint>

namespace pxr
{
	struct VertexBufferElement
	{
		Type Type;
		uint32_t Count;
		bool Normalized;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, uint32_t size, bool dynamic = false);
		~VertexBuffer();

		void PushData(const void* data, uint32_t size) const;

		void Bind() const;
		void Unbind() const;

		void SetLayout(const std::vector<VertexBufferElement>& elements) { m_LayoutElements = elements; }
		inline const std::vector<VertexBufferElement>& GetLayoutElements() const { return m_LayoutElements; }
	private:
		uint32_t m_RendererID = 0;

		std::vector<VertexBufferElement> m_LayoutElements;
	};
}