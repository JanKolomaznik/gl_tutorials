#version 430 core

in vec3 in_vert;
in vec3 in_normal;
out vec3 out_normal;

void main() {
	gl_Position = vec4(in_vert, 1.0);
	out_normal = in_normal;
}
