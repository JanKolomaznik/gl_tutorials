#version 430 core


in vec3 f_position; // Fragment position passed in from vertex shader
uniform vec3 u_viewPos; // Camera position
uniform sampler3D u_volumeData; // 3D texture containing the volume data
uniform mat4 u_invModelMat; // Inverse model matrix
uniform float u_intensityMultiplier = 1.0;

uniform float stepSize = 0.01; // Sampling step size along the ray

out vec4 out_fragColor; // Output fragment color

vec3 computeExitPoint(vec3 point1, vec3 point2, vec3 entryPoint, vec3 rayDir) {
	vec3 invDir = 1.0 / rayDir; // Inverse of the ray direction
	vec3 tMin = (point1 - entryPoint) * invDir;
	vec3 tMax = (point2 - entryPoint) * invDir;

	// When ray direction component is negative, swap tMin and tMax for that component
	vec3 t = max(tMin, tMax);

	float tExit = min(min(t.x, t.y), t.z);

	// Compute the exit point using tExit
	vec3 exitPoint = entryPoint + tExit * rayDir;

	return exitPoint;
}

// We assume that we are mapping cube [-0.5, 0.5]
vec3 positionToTexCoords(vec3 position) {
	return position + vec3(0.5);
}

void main() {
	// Initialize maximum intensity to a low value
	float maxIntensity = 0.0;

	// Compute the ray direction in world space
	vec3 rayDir = normalize(f_position - u_viewPos);
	// Transform the ray direction into the model's local space
	vec3 localRayDir = normalize((u_invModelMat * vec4(rayDir, 0.0)).xyz);

	// Compute intersection points in the local coordinates
	vec4 pos = u_invModelMat * vec4(f_position, 1.0);
	vec3 entryPoint = pos.xyz / pos.w;
	vec3 exitPoint = computeExitPoint(vec3(-0.5), vec3(0.5), entryPoint,localRayDir);

	// How many steps will fit into the line between entry and exit
	float distance = length(entryPoint - exitPoint);
	int stepCount = min(1000, int(distance/stepSize));

	for (int i = 0; i < stepCount; ++i) {
		vec3 sampleCoords = entryPoint + (i * 0.01 * localRayDir);
		vec3 texCoords = positionToTexCoords(sampleCoords);
	 	float intensity = texture(u_volumeData, texCoords).r;
		maxIntensity = max(maxIntensity, intensity);
	}
	// Set the fragment color to the maximum intensity found, encoded as grayscale, multiplied to get into visible range
	out_fragColor = vec4(vec3(u_intensityMultiplier * maxIntensity), 1.0);
}
