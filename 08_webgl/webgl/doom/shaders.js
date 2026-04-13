export const vertexShaderSrc = `#version 300 es
precision highp float;

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;
uniform mat3 u_normalMat;

in vec3 in_vert;
in vec3 in_normal;
in vec2 in_texCoord;

out vec3 f_normal;
out vec3 f_position;
out vec2 f_texCoord;

void main() {
  vec4 worldPosition = u_modelMat * vec4(in_vert, 1.0);
  f_position = worldPosition.xyz;
  f_normal = normalize(u_normalMat * in_normal);
  f_texCoord = in_texCoord;
  gl_Position = u_projMat * u_viewMat * worldPosition;
}`;

export const fragmentShaderEnemySrc = `#version 300 es
precision mediump float;

uniform sampler2D u_texture;
in vec3 f_normal;
in vec3 f_position;
in vec2 f_texCoord;

out vec4 fragColor;

void main() {
	fragColor = texture(u_texture, f_texCoord);
}`;

export const fragmentShaderFloorSrc = `#version 300 es
precision mediump float;

uniform sampler2D u_texture;
in vec3 f_normal;
in vec3 f_position;
in vec2 f_texCoord;

out vec4 fragColor;

void main() {
	fragColor = texture(u_texture, 100.0*f_texCoord);
}`;
