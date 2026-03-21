#version 330 core

layout (location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_Emission;

uniform sampler2D u_Textures[32];

void main()
{
	int index = int(v_TexIndex);
	vec4 texColor = texture(u_Textures[index], v_TexCoord) * v_Color * v_Emission;

	if (texColor.a == 0.0f)
		discard;

	o_Color = texColor;
}