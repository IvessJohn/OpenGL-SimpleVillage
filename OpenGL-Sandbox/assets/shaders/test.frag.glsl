#version 450 core

layout (location = 0) out vec4 o_Color;

in vec4 v_Color;

uniform vec4 u_Color;

void main()
{
	//o_Color = u_Color;
	o_Color = v_Color;
}