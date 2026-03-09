#version 430 core


struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 f_normal;// Normal vector in world space (set by vertex shader)
in vec3 f_position;// Fragment position in world space (set by vertex shader)
in vec2 f_texCoord;
uniform vec3 u_viewPos; // Camera/view position in world space

// Uniforms with default values
uniform Material material = Material(
		vec3(0.1, 0.1, 0.1), // ambient
		vec3(1.0, 0.5, 0.31), // diffuse
		vec3(0.5, 0.5, 0.5),  // specular
		32.0                  // shininess
		);

uniform Light light = Light(
		vec3(3.0, 5.0, 6.0),  // position
		vec3(0.3, 0.3, 0.3),  // ambient
		vec3(0.6, 0.6, 0.6),  // diffuse
		vec3(1.0, 1.0, 1.0)   // specular
		);

out vec4 out_fragColor;

void main() {
	// Ambient
	vec3 ambient = light.ambient * material.ambient;

	// Diffuse 
	vec3 norm = normalize(f_normal);
	vec3 lightDir = normalize(light.position - f_position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// Specular
	vec3 viewDir = normalize(u_viewPos - f_position);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//vec3 specular = light.specular * (spec * material.specular);
	vec3 specular = light.specular * (spec * material.specular * diff);

	vec3 result = ambient + diffuse + specular;
	out_fragColor = vec4(result, 1.0);
}

