#version 430 core

uniform sampler2D u_textureSampler;

out vec4 out_fragColor;

in vec3 f_normal;
in vec2 f_texCoord;

void main() {
	out_fragColor = texture(u_textureSampler, f_texCoord);
	// out_fragColor = vec4(f_texCoord, 0, 1);
}

