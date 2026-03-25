#version 330 core

layout(location = 0) out vec4 color;
in vec2 texCoord;

uniform sampler2D u_Texture;

// This shader uses ACES tonemapping
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/

void main()
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;

    vec4 x = texture(u_Texture, texCoord);
    color = (x*(a*x+b))/(x*(c*x+d)+e);
}