#version 430 core

layout(local_size_x = 256) in;

layout(std430, binding = 0) buffer Data {
	float elements[];
};

uniform float multiplier;

void main() {
	int idx = int(gl_GlobalInvocationID.x);
	elements[idx] *= multiplier;
}
