#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_ColorTexCoord;
layout (location = 3) in float a_ColorTexIndex;
layout (location = 4) in float a_Emission;
layout (location = 5) in vec2 a_EmissionTexCoord;
layout (location = 6) in float a_EmissionTexIndex;

uniform mat4 u_ViewProj;
uniform mat4 u_Transform;

out vec4 v_Color;
out vec2 v_ColorTexCoord;
out float v_ColorTexIndex;
out float v_Emission;
out vec2 v_EmissionTexCoord;
out float v_EmissionTexIndex;

void main()
{
	v_Color = a_Color;
	v_ColorTexCoord = a_ColorTexCoord;
	v_ColorTexIndex = a_ColorTexIndex;
	v_Emission = a_Emission;
	v_EmissionTexCoord = a_EmissionTexCoord;
	v_EmissionTexIndex = a_EmissionTexIndex;
	gl_Position = u_ViewProj * u_Transform * vec4(a_Position, 1.0);
}