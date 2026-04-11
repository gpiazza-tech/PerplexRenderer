#version 330 core

layout (location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec2 v_ColorTexCoord;
in float v_ColorTexIndex;
in float v_Emission;
in vec2 v_EmissionTexCoord;
in float v_EmissionTexIndex;

uniform sampler2D u_Textures[32];

void main()
{
	int colorIndex = int(v_ColorTexIndex);
	vec4 texColor = texture(u_Textures[colorIndex], v_ColorTexCoord) * v_Color;

	if (texColor.a == 0.0f)
		discard;

	int emissionIndex = int(v_EmissionTexCoord);
	vec4 texEmission = texture(u_Textures[emissionIndex], v_EmissionTexCoord) * v_Emission;

	o_Color = texColor + texEmission;
}