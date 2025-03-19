#version 430 core

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;
uniform mat3 u_normalMat;

in vec3 in_vert;
in vec3 in_normal;
in vec2 in_texCoord;

out vec4 position;

void main(void)
{
	gl_Position = u_projMat * u_viewMat * u_modelMat * vec4(in_vert, 1);
}

