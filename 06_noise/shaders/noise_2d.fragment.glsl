#version 430 core
#include "perlin"
#include "simplex"
#include "worley"

out vec4 out_fragColor;

in vec3 f_normal;
in vec2 f_texCoord;

uniform float u_noiseScale = 10.0;

uniform int u_selectedNoise = 0;

float noise_2d(vec2 coords) {
	if (u_selectedNoise == 0) {
		return cnoise_2d(coords);
	}
	if (u_selectedNoise == 1) {
		return snoise_2d(coords);
	}
	if (u_selectedNoise == 2) {
		return cellular(coords).x;
	}
	return 0.0;
}

void main() {
	vec2 coords = u_noiseScale*f_texCoord;
	float val = 0.5 + 0.5 * noise_2d(coords);
	out_fragColor = vec4(val, val, val, 1.0);
}

