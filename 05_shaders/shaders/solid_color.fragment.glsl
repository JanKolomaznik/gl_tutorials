#version 430 core

uniform vec4 u_solidColor = vec4(0.0, 0.0, 1.0, 1.0);

out vec4 out_fragColor;

void main() {
	out_fragColor = u_solidColor;
}

