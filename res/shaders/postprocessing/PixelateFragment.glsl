#version 330 core

layout(location = 0) out vec3 color;
in vec2 texCoord;

uniform sampler2D u_Texture;

// This downsamples linearly to produce a pixelated output

void main()
{
    float pixelX = 128;
    float pixelY = 72;

    float x = float(int(texCoord.x * pixelX)) / pixelX;
    float y = float(int(texCoord.y * pixelY)) / pixelY;

    float pixelRadius = 1.0f / pixelX / 2.0f;

    vec4 texColor = texture(u_Texture, vec2(x + pixelRadius, y + pixelRadius));
    color = vec3(texColor.rgb);

    // vec2 scaledCoord = texCoord * 100.0f;
    // ivec2 iCoord = ivec2(scaledCoord);
    // vec2 sampleCoord = vec2(iCoord) / 100.0f;
    // 
    // vec4 texColor = texture(u_Texture, sampleCoord);
    // color = vec3(texColor.rgb);
}