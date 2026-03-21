#version 330 core

layout(location = 0) out vec4 color;
in vec2 texCoord;

uniform sampler2D u_Texture;

// This shader uses local Reinhard tonemapping: out = in / (in + 1);

void main()
{
    vec4 texColor = texture(u_Texture, texCoord);
    color = texColor / (texColor + 1);
}