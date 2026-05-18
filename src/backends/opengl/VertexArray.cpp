#include <pxr/pch.h>
#include <pxr/backends/VertexArray.h>

#include <pxr/util/Type.h>
#include <pxr/backends/VertexBuffer.h>
#include <pxr/backends/IndexBuffer.h>

#include <GL/glew.h>

#include <vector>
#include <cstdint>

namespace pxr
{
	static GLenum PxrToGlType(Type type)
	{
		switch (type)
		{
		case Type::Int:
			return GL_UNSIGNED_INT;
		case Type::Float:
			return GL_FLOAT;
		case Type::Byte:
			return GL_UNSIGNED_BYTE;
		default:
			std::cout << "pxr::Type not implemented!" << std::endl;
			return 0;
		}
	}

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &m_RendererID);
	}
	
	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::AttachBuffers(const VertexBuffer& vbo, const IndexBuffer& ibo)
	{
		Bind();
		vbo.Bind();
		ibo.Bind();
		const auto& elements = vbo.GetLayoutElements();
		size_t offset = 0;
		size_t stride = 0;

		for (size_t i = 0; i < elements.size(); i++)
		{
			stride += elements[i].Count * SizeOf(elements[i].Type);
		}

		for (size_t i = 0; i < elements.size(); i++)
		{
			const auto& element = elements[i];
			glEnableVertexAttribArray((GLuint)i);
			glVertexAttribPointer((GLuint)i, element.Count, PxrToGlType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE, (GLsizei)stride, (const void*)offset);
			offset += element.Count * SizeOf(element.Type);
		}
		Unbind();
		vbo.Unbind();
		ibo.Unbind();
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
}