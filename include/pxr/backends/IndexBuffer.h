#pragma once

#include <cstdint>

namespace pxr
{
	class IndexBuffer
	{
	public:
		IndexBuffer(const uint32_t* data, uint32_t count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		inline const uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Count = 0;
	};
}