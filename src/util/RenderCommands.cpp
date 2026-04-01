#include <pch.h>
#include "RenderCommands.h"

#include "ScreenQuad.h"

#include <GL/glew.h>

namespace pxr
{
    void RenderCommands::DrawScreen()
    {
        glDisable(GL_DEPTH_TEST);

        static ScreenQuad screenQuad;
        screenQuad.Draw();

        glEnable(GL_DEPTH_TEST);
    }

    void RenderCommands::ResizeViewport(int width, int height)
    {
        glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    }
}