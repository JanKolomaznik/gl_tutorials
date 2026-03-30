#version 430 core

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;

in vec3 in_vert;
in vec3 in_color;

out vec3 f_color;

void main(void)
{
	gl_Position = u_projMat * u_viewMat * u_modelMat * vec4(in_vert, 1);
	f_color = in_color;
}

