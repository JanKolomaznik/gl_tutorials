#version 430 core

in vec3 f_normal;

out vec4 out_fragColor;

void main() {
	out_fragColor = vec4(0.5 * f_normal + vec3(0.5, 0.5, 0.5), 1);
}

