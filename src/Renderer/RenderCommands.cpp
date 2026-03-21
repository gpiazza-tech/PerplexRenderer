#include "RenderCommands.h"

#include "ScreenQuad.h"

#include <gl/glew.h>

void RenderCommands::DrawScreen()
{
    glDisable(GL_DEPTH_TEST);

    static ScreenQuad screenQuad;
    screenQuad.Draw();

    glEnable(GL_DEPTH_TEST);
}