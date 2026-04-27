#version 430 core

layout(binding = 0) uniform sampler2D u_diffuseTexture;
layout(binding = 1) uniform sampler2D u_shadowMap;
/* layout(binding = 1) uniform sampler2D u_specularTexture; */
/* layout(binding = 2) uniform sampler2D u_normalTexture; */
/* layout(binding = 3) uniform sampler2D u_displacementTexture; */
/* layout(binding = 4) uniform sampler2D u_ambientOccTexture; */

layout(location = 15) uniform vec3 u_lightPos;
layout(location = 20) uniform mat4 u_lightMat;
layout(location = 40) uniform mat4 u_lightProjMat;

in vec4 position;
in vec2 texCoords;
in vec3 normal;

out vec4 out_color;
out vec3 out_normal;
out vec3 out_position;
out vec3 out_shadow;

vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845),
    vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554),
    vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507),
    vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367),
    vec2(0.14383161, -0.14100790)
);

void main() {
	out_color = texture(u_diffuseTexture, texCoords);
	out_normal = normalize(normal);
	out_position = position.xyz/position.w;

	vec4 shadowCoords = (u_lightProjMat * u_lightMat * position);
	vec3 projCoords = shadowCoords.xyz / shadowCoords.w;
	projCoords = projCoords * 0.5 + 0.5; // Transform from NDC to [0,1]

	float shadowFactor = 1.0;

	if (projCoords.x >= 0.0 && projCoords.x <= 1.0 &&
			projCoords.y >= 0.0 && projCoords.y <= 1.0 &&
			projCoords.z >= 0.0 && projCoords.z <= 1.0) {

		float currentDepth = clamp(projCoords.z, 0.0, 1.0);
		float bias = max(0.0005 * (1.0 - dot(normalize(normal), normalize(u_lightPos - out_position))), 0.0002);
		float visibility = 0.0;
		float texelSize = 1.0 / textureSize(u_shadowMap, 0).x;

		for (int i = 0; i < 16; ++i) {
			vec2 offset = poissonDisk[i] * texelSize * 1.5;
			float closestDepth = texture(u_shadowMap, projCoords.xy + offset).r;
			if (currentDepth - bias <= closestDepth) {
				visibility += 1.0;
			}
		}

		shadowFactor = visibility / 16.0;
	}

	out_shadow = vec3(shadowFactor);
}
