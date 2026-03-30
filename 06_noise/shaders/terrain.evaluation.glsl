#version 430 core
#include "perlin"
#include "simplex"

layout(triangles, equal_spacing, ccw) in;

in TCS_OUT {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} tes_in[];

out vec3 f_position;
out vec2 f_texCoord;

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;

uniform float u_noiseScale = 10.0;
uniform int u_selectedNoise = 0;
uniform float u_maxDisplacement = 0.2;

float noise_2d(vec2 coords) {
	if (u_selectedNoise == 0) {
		return cnoise_2d(coords);
	}
	if (u_selectedNoise == 1) {
		return snoise_2d(coords);
	}
	return 0.0;
}

void main() {
    vec3 p0 = gl_TessCoord.x * tes_in[0].position;
    vec3 p1 = gl_TessCoord.y * tes_in[1].position;
    vec3 p2 = gl_TessCoord.z * tes_in[2].position;
    vec3 position = p0 + p1 + p2;

    vec3 n0 = gl_TessCoord.x * tes_in[0].normal;
    vec3 n1 = gl_TessCoord.y * tes_in[1].normal;
    vec3 n2 = gl_TessCoord.z * tes_in[2].normal;
    vec3 normal = normalize(n0 + n1 + n2);

    f_texCoord = gl_TessCoord.x * tes_in[0].texCoords +
                gl_TessCoord.y * tes_in[1].texCoords +
                gl_TessCoord.z * tes_in[2].texCoords;

    float noise = noise_2d(u_noiseScale*f_texCoord);
    position += noise * normal * u_maxDisplacement;

    f_position = vec3(u_modelMat * vec4(position, 1.0));

    gl_Position = u_projMat * u_viewMat * u_modelMat * vec4(position, 1.0);
}
