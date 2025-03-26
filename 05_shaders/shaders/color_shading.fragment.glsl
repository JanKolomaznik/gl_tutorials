#version 430 core

const vec3 u_lightPosition = vec3(15.0, 20.0, -10.0);

in vec3 f_normal;
in vec3 f_position;
in vec3 f_color;

out vec4 out_fragColor;

void main() {
	vec3 lightDir = normalize(u_lightPosition - f_position);
	float lambertian = max(dot(lightDir, f_normal), 0.0);
	out_fragColor = vec4((f_color * lambertian), 1.0);
}

