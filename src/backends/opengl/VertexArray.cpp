#include <pch.h>
#include <backends/VertexArray.h>

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

	void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
	{
		Bind();
		vb.Bind();
		const auto& elements = layout.GetElements();
		uint32_t offset = 0;
		for (size_t i = 0; i < elements.size(); i++)
		{
			const auto& element = elements[i];
			glEnableVertexAttribArray((GLuint)i);
			glVertexAttribPointer((GLuint)i, element.Count, PxrToGlType(element.Type), 
				element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)offset);
			offset += element.Count * SizeOf(element.Type);
		}
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