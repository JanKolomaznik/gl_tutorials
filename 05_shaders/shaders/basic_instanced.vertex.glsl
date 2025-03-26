#version 430 core

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;
uniform mat3 u_normalMat;

layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texCoord;

layout(location = 3) in vec3 in_offset;
layout(location = 4) in vec3 in_color;

out vec3 f_normal;
out vec3 f_position;
out vec2 f_texCoord;
out vec3 f_color;

void main(void)
{
	gl_Position = u_projMat * u_viewMat * u_modelMat * vec4(in_vert + in_offset, 1);
	f_normal = normalize(u_normalMat * in_normal);
	f_position = vec3(u_modelMat * vec4(in_vert + in_offset, 1.0));
	f_texCoord = in_texCoord;
	f_color = in_color;
}

