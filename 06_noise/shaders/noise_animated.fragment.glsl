#version 430 core
#include "perlin"
#include "simplex"

out vec4 out_fragColor;

uniform float u_elapsedTime = 0.0;

in vec3 f_normal;
in vec2 f_texCoord;

uniform float u_noiseScale = 10.0;
uniform int u_selectedNoise = 0;

float noise_3d(vec3 coords) {
	if (u_selectedNoise == 0) {
		return cnoise_3d(coords);
	}
	if (u_selectedNoise == 1) {
		return snoise_3d(coords);
	}
	return 0.0;
}

void main() {
	vec3 coords = vec3(u_noiseScale*f_texCoord, u_elapsedTime);
	float val = 0.5 + 0.5*noise_3d(coords);
	out_fragColor = vec4(val, val, val, 1.0);
}

