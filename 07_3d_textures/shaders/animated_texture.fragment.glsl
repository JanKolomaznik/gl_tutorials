#version 430 core


in vec2 f_texCoord; // Fragment position passed in from vertex shader
uniform sampler3D u_volumeData; // 3D texture containing the volume data
uniform float u_elapsedTime = 0.0;
uniform float u_speed = 1.0;
uniform float u_intensityMultiplier = 1.0;

out vec4 out_fragColor; // Output fragment color

void main() {
	float time = u_speed * u_elapsedTime;
	float newCoord = abs(time - int(time / 2.0) * 2 - 1.0);
	vec3 texCoords = vec3(f_texCoord, newCoord);
	float intensity = texture(u_volumeData, texCoords).r;
	// multiplied to get into visible range
	out_fragColor = vec4(vec3(u_intensityMultiplier * intensity), 1.0);
}
