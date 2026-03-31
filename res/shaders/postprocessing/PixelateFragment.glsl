#version 330 core

layout(location = 0) out vec3 color;
in vec2 texCoord;

uniform sampler2D u_Texture;
uniform vec2 u_TargetResolution;

// This downsamples linearly to produce a pixelated output

void main()
{
    float x = float(int(texCoord.x * u_TargetResolution.x)) / u_TargetResolution.x;
    float y = float(int(texCoord.y * u_TargetResolution.y)) / u_TargetResolution.y;

    float pixelRadius = 1.0f / u_TargetResolution.x / 2.0f;

    vec4 texColor = texture(u_Texture, vec2(x + pixelRadius, y + pixelRadius));
    // vec4 texColor = texture(u_Texture, vec2(x, y));
    color = vec3(texColor.rgb);
}