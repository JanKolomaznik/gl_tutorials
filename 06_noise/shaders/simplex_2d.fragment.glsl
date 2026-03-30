#version 430 core
#include "simplex"

out vec4 out_fragColor;

in vec3 f_normal;
in vec2 f_texCoord;

uniform float u_noiseScale = 10.0;

void main() {
	float val = 0.5 + 0.5*snoise_2d(u_noiseScale*f_texCoord);
	out_fragColor = vec4(val, val, val, 1.0);
}
