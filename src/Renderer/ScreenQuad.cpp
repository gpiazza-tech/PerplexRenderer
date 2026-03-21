#include "ScreenQuad.h"

#include <gl/glew.h>

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

    // Vertex array
    glGenVertexArrays(1, &m_Vao);
    glBindVertexArray(m_Vao);

    // Array buffer
    uint32_t screenABO;
    glGenBuffers(1, &screenABO);
    glBindBuffer(GL_ARRAY_BUFFER, screenABO);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), screenVertices, GL_STATIC_DRAW);

    // Array attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Index buffer
    uint32_t screenIBO;
    glGenBuffers(1, &screenIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

ScreenQuad::~ScreenQuad()
{
    glDeleteBuffers(1, &m_Vao);
}

void ScreenQuad::Draw()
{
    glBindVertexArray(m_Vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}