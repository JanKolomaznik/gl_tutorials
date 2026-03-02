#pragma once


#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
};


struct VertexNorm {
	glm::vec3 position;
	glm::vec3 normal;
};

struct VertexColor {
	glm::vec3 position;
	glm::vec3 color;
};


struct VertexTex {
	glm::vec3 position;
	glm::vec2 texCoords;
};

struct VertexNormTex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};
