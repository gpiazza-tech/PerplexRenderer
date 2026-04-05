#include <pch.h>
#include <backends/VertexBuffer.h>

#include <GL/glew.h>

#include <cstdint>

namespace pxr
{
	VertexBuffer::VertexBuffer(const void* data, uint32_t size, bool dynamic)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{	
		glDeleteBuffers(1, &m_RendererID);
	}

	void VertexBuffer::PushData(const void* data, uint32_t size) const
	{
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}