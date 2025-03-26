#version 430 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in vec3 out_normal[]; // Output from vertex shader for each vertex

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;
uniform mat3 u_normalMat;

void main()
{
	vec3 normal = 0.3 * normalize(u_normalMat * out_normal[0]);
	vec4 position = u_modelMat * gl_in[0].gl_Position;
	gl_Position = u_projMat *  u_viewMat *  position;
	EmitVertex();

	position += vec4(normal, 0.0);
	gl_Position = u_projMat *  u_viewMat * position;
	EmitVertex();

	EndPrimitive();
}
