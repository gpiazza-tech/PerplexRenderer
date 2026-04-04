#pragma once

#include <util/Type.h>

#include <cstdint>
#include <vector>

namespace pxr
{
	struct VertexBufferElement
	{
		Type Type;
		uint32_t Count;
		bool Normalized;
	};

	class VertexBufferLayout
	{
	public:
		void Push(Type type, uint32_t count, bool normalized = false)
		{
			m_Elements.push_back({ type, count, normalized });
			m_Stride += count * SizeOf(type);
		}

		inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
		inline uint32_t GetStride() const { return m_Stride; }
	private:
		std::vector<VertexBufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};
}