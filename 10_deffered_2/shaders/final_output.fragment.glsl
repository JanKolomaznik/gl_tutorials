#version 430 core

layout(binding = 0) uniform sampler2D u_diffuse;

in vec2 texCoords;

out vec4 fragColor;

void main() {
	vec3 diffuseColor = texture(u_diffuse, texCoords).xyz;
	fragColor = vec4(diffuseColor, 1.0);
}

