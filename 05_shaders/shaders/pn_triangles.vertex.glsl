#version 400 compatibility
in vec3 in_vert;
in vec3 in_normal;

out vec3 Normal_CS_in;
out vec3 WorldPos_CS_in;

uniform mat4 u_modelMat;
/*uniform mat4 u_viewMat;
uniform mat4 u_projMat;
uniform mat3 u_normalMat;*/

void
main(){
	vec3 xyz = in_vert;
	WorldPos_CS_in = (u_modelMat * vec4( xyz, 1. )).xyz;
	Normal_CS_in = normalize( (u_modelMat * vec4(in_normal, 0.0)).xyz );
}
