#version 330 core

layout(location = 0) out vec4 color;
in vec2 texCoord;

uniform sampler2D u_Texture;
uniform float u_Threshold;

void main()
{
	vec4 texColor = texture(u_Texture, texCoord);

	// function
	texColor = texColor - u_Threshold;

	color = texColor;
}