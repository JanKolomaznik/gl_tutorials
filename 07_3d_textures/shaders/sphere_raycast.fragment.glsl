#version 430 core


in vec3 f_position; // Fragment position in world space
out vec4 out_fragColor;

uniform vec3 u_viewPos; // Camera position in world space
uniform mat4 u_invModelMat; // Inverse model matrix
uniform mat3 u_normalMat;
			    
const vec3 sphereCenter = vec3(0.0); // Sphere center in world space
const float sphereRadius = 0.5; // Sphere radius
const vec3 lightDir = normalize(vec3(1.0, 1.0, -1.0)); // Example light direction
						 
void main() {
	// Compute the ray direction in world space
	vec3 rayDir = normalize(f_position - u_viewPos);
	// Transform the ray direction into the model's local space
	vec3 localRayDir = normalize((u_invModelMat * vec4(rayDir, 0.0)).xyz);

	// Compute intersection points in the local coordinates
	vec4 pos = u_invModelMat * vec4(f_position, 1.0);
	vec3 entryPoint = pos.xyz / pos.w;

	vec3 oc = entryPoint - sphereCenter;
	float a = dot(localRayDir, localRayDir);
	float b = 2.0 * dot(oc, localRayDir);
	float c = dot(oc, oc) - sphereRadius * sphereRadius;
	float discriminant = b * b - 4 * a * c;

	// Sphere equation: (p - c)^2 = r^2, where p is a point on the sphere, c is the sphere center, and r is the radius
	// Ray equation: p = o + td, where o is the ray origin, d is the normalized ray direction, and t is a scalar
	// Substituting the ray equation into the sphere equation gives a quadratic equation of form: at^2 + bt + c = 0
	if (discriminant > 0) {
		// Ray intersects the sphere, compute the distance t to the intersection point
		float t = (-b - sqrt(discriminant)) / (2.0 * a);
		if (t > 0.0) {
			// Compute the normal at the intersection point for simple lighting
			vec3 hitPoint = entryPoint + t * localRayDir;
			vec3 normal = normalize(u_normalMat * (hitPoint - sphereCenter));	

			float diff = max(dot(normal, lightDir), 0.0);
			vec3 color = diff * vec3(0.5, 0.8, 1.0); // Example color

			out_fragColor = vec4(color, 1.0);
			return;
		}
	}

	// If there's no intersection, or the intersection is behind the ray origin, discard the fragment
	discard;
}
