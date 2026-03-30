#version 430 core

in vec3 in_vert;
in vec3 in_normal;
in vec2 in_texCoord;

out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vs_out;

void main() {
    vs_out.position = in_vert;
    vs_out.normal = in_normal;
    vs_out.texCoords = in_texCoord;
}
