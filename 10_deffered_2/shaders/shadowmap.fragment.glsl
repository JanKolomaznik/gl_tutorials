#version 430 core

out vec4 out_fragColor;

float linearizeDepth(float depth) {
	// TODO - use setting for your projection matrix
	float near = 0.01; // Camera's near plane
	float far = 100.0;  // Camera's far plane

	return (2.0 * near) / (far + near - depth * (far - near));
}

void main() {
	float z = gl_FragCoord.z; // Non-linear depth value
	float linearDepth = linearizeDepth(z);
	// store the fragment depth and also its linearized value - useful for debugging
	out_fragColor = vec4(z, linearDepth, 0.0, 1.0);
}

