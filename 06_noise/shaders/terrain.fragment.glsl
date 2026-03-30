#version 430 core

const vec3 u_lightPosition = vec3(15.0, 20.0, -10.0);

out vec4 out_fragColor;

in vec3 f_position;
in vec2 f_texCoord;

void main() {
	vec3 lightDir = normalize(u_lightPosition - f_position);
	vec3 normal = normalize(cross(dFdx(f_position), dFdy(f_position)));
	float lambertian = max(dot(lightDir, normal), 0.0);
	out_fragColor = vec4(vec3(0.0, 1.0, 0.0) * lambertian, 1.0);
}

