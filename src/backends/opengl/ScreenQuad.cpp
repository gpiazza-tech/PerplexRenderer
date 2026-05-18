#include <pxr/pch.h>
#include <pxr/backends/ScreenQuad.h>

#include <pxr/backends/IndexBuffer.h>
#include <pxr/backends/VertexBuffer.h>
#include <pxr/backends/VertexArray.h>
#include <pxr/backends/RenderCommands.h>
#include <pxr/util/Type.h>

#include <cstdint>

namespace pxr
{
    ScreenQuad::ScreenQuad()
    {
        float screenVertices[16] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 1.0f
        };

        uint32_t indices[6] = {
            0, 1, 2,
            2, 3, 0
        };

        VertexBuffer vbo((void*)screenVertices, 16 * sizeof(float));
        vbo.SetLayout(
            {
                { Type::Float, 2, false },
                { Type::Float, 2, false },
            });
        
        // attach index buffer
        m_VAO.Bind();
        IndexBuffer ibo(indices, 6);
        ibo.Bind();
        
        m_VAO.AttachBuffers(vbo, ibo);
        
        m_VAO.Unbind();
        ibo.Unbind();
        vbo.Unbind();
    }

    void ScreenQuad::Draw() const
    {
        RenderCommands::DrawTriangles(m_VAO, 6);
    }
}