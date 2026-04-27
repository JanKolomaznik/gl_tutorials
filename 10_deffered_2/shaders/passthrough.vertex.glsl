#version 430 core

layout (location = 0) in vec3 in_vert;
layout (location = 1) in vec2 in_texCoords;

out vec2 texCoords;

void main() {
	gl_Position = vec4(in_vert, 1.0);
	texCoords = in_texCoords;
}

