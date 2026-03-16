#version 430 core

layout(binding = 0) uniform sampler2D u_diffuseTexture;
layout(binding = 1) uniform sampler2D u_specularTexture;
layout(binding = 2) uniform sampler2D u_normalTexture;
layout(binding = 3) uniform sampler2D u_displacementTexture;
layout(binding = 4) uniform sampler2D u_ambientOccTexture;

in vec4 position;
in vec2 texCoords;
in vec3 normal;

out vec4 out_color;
out vec3 out_normal;
out vec3 out_position;

const float near = 0.01;
const float far = 100.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Transform [0,1] depth to [-1,1] NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	out_color = texture(u_diffuseTexture, texCoords);
	out_normal = normalize(normal);
	//out_position = position.xyz/position.w;
	out_position = vec3(gl_FragCoord.z, LinearizeDepth(gl_FragCoord.z), 0.0);
}
