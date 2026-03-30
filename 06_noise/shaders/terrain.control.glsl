#version 430 core

layout(vertices = 3) out;

in VS_OUT {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
} tcs_in[];

out TCS_OUT {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
} tcs_out[];

uniform int u_outer = 2, u_inner = 2;

void main() {
	if (gl_InvocationID == 0) {
		gl_TessLevelInner[0] = u_inner;
		gl_TessLevelOuter[0] = u_outer;
		gl_TessLevelOuter[1] = u_outer;
		gl_TessLevelOuter[2] = u_outer;
	}
	tcs_out[gl_InvocationID].position = tcs_in[gl_InvocationID].position;
	tcs_out[gl_InvocationID].normal = tcs_in[gl_InvocationID].normal;
	tcs_out[gl_InvocationID].texCoords = tcs_in[gl_InvocationID].texCoords;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
