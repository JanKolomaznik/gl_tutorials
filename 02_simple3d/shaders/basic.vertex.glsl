#version 430 core

layout(location = 2) uniform mat4 u_modelMat;
layout(location = 30) uniform mat4 u_viewMat;
layout(location = 60) uniform mat4 u_projMat;

in vec3 in_vert;

void main(void)
{
	gl_Position = u_projMat * u_viewMat * u_modelMat * vec4(in_vert, 1);
	/* gl_Position = u_viewMat * u_modelMat * vec4(in_vert, 1); */
}

