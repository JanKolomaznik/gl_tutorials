#pragma once

#include <vector>
#include "ogl_resource.hpp"
#include "obj_file_loading.hpp"

// struct IndexedBuffer {
// 	OpenGLResource vbo;
// 	OpenGLResource ebo;
// 	OpenGLResource vao;
//
// 	unsigned int indexCount = 0;
// 	GLenum mode = GL_TRIANGLES;
// };
using VBOVector = std::vector<OpenGLResource>;
struct IndexedBuffer {
	OpenGLResource vao;
	std::vector<OpenGLResource> vbos;
	unsigned int indexCount = 0;
	unsigned int instanceCount = 0;
	GLenum mode = GL_TRIANGLES;
};

inline glm::vec3 insertDimension(const glm::vec2& v, int dimension, float value) {
	switch (dimension) {
		case 0: return glm::vec3(value, v.x, v.y); // Insert before x
		case 1: return glm::vec3(v.x, value, v.y); // Insert between x and y
		case 2: return glm::vec3(v, value);        // Insert after y
		default: throw std::out_of_range("Dimension must be between 0 and 2");
	}
}

extern const std::array<glm::vec2, 4> unitFaceVertices;
extern const std::array<unsigned int, 6> faceTriangleIndices;

IndexedBuffer
generateAxisGizmo();

IndexedBuffer
generateQuadTex();

IndexedBuffer
generateCubeOutlineBuffers();

IndexedBuffer
generateCubeBuffers();

IndexedBuffer
generateCubeBuffersNormTex();

IndexedBuffer
generatePlaneOutlineBuffers();

IndexedBuffer
generatePlaneBuffers();

IndexedBuffer
generateMeshBuffersNormTex(const ObjMesh &aMesh);

IndexedBuffer
generateQuadMeshBuffersNormTex(const ObjMesh &aMesh);
