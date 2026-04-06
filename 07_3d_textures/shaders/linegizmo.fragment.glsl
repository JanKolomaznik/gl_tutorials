#version 430 core

in vec3 f_color;

out vec4 out_fragColor;

void main() {
	out_fragColor = vec4(f_color, 1.0);
}

