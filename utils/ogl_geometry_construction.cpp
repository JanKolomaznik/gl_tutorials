#include "ogl_geometry_construction.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <array>
#include "vertex.hpp"

//glPrimitiveRestartIndex(0xFFFFFFFF);

static const std::array<glm::vec2, 4> unitFaceVertices = {
	// Positions
	glm::vec2( -0.5f, -0.5f ),
	glm::vec2(  0.5f, -0.5f ),
	glm::vec2( -0.5f,  0.5f ),
	glm::vec2(  0.5f,  0.5f )
};

static const std::array<unsigned int, 6> faceTriangleIndices = {
	0, 1, 3, 0, 3, 2,
};

const float cubeVertices[] = {
	// Positions
	-0.5f, -0.5f, -0.5f,  // 0. Back face
	0.5f, -0.5f, -0.5f,  // 1.
	-0.5f,  0.5f, -0.5f,  // 2.
	0.5f,  0.5f, -0.5f,  // 3.
	-0.5f, -0.5f,  0.5f,  // 4. Front face
	0.5f, -0.5f,  0.5f,  // 5.
	-0.5f,  0.5f,  0.5f,  // 6.
	0.5f,  0.5f,  0.5f   // 7.
};

glm::vec3 insertDimension(const glm::vec2& v, int dimension, float value) {
	switch (dimension) {
		case 0: return glm::vec3(value, v.x, v.y); // Insert before x
		case 1: return glm::vec3(v.x, value, v.y); // Insert between x and y
		case 2: return glm::vec3(v, value);        // Insert after y
		default: throw std::out_of_range("Dimension must be between 0 and 2");
	}
}

IndexedBuffer
generateAxisGizmo() {
	IndexedBuffer buffers {
		createBuffer(),
		createBuffer(),
		createVertexArray(),
		0
	};
	// Gizmo vertex data: positions and colors (R, G, B for X, Y, Z axes)
	std::vector<VertexColor> gizmoVertices = {
		// Positions    // Colors
		{{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // X-axis (red)
		{{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Y-axis (green)
		{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // Z-axis (blue)
		{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}
	};
	std::vector<unsigned int> indices = {
		0, 1,
		2, 3,
		4, 5
	};
	GL_CHECK(glBindVertexArray(buffers.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo.get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexColor) * gizmoVertices.size(), gizmoVertices.data(), GL_STATIC_DRAW));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo.get()));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW));

	// Position attribute
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(indices), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	// Texture coordinate attribute
	GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(indices), (void*)(sizeof(glm::vec3))));
	GL_CHECK(glEnableVertexAttribArray(1));

	// Unbind VAO
	GL_CHECK(glBindVertexArray(0));

	buffers.indexCount = unsigned(indices.size());
	buffers.mode = GL_LINES;
	return buffers;

}




static const std::array<VertexTex, 4> quadVertices = {
	VertexTex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
	VertexTex(glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
	VertexTex(glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
	VertexTex(glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec2(1.0f, 1.0f))
};

IndexedBuffer
generateQuadTex() {
	IndexedBuffer buffers {
		createBuffer(),
		createBuffer(),
		createVertexArray(),
		0
	};
	GL_CHECK(glBindVertexArray(buffers.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo.get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTex) * quadVertices.size(), quadVertices.data(), GL_STATIC_DRAW));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo.get()));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faceTriangleIndices.size(), faceTriangleIndices.data(), GL_STATIC_DRAW));

	// Position attribute
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTex), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	// Texture coordinate attribute
	GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTex), (void*)(sizeof(glm::vec3))));
	GL_CHECK(glEnableVertexAttribArray(1));

	// Unbind VAO
	GL_CHECK(glBindVertexArray(0));

	buffers.indexCount = unsigned(faceTriangleIndices.size());
	buffers.mode = GL_TRIANGLES;
	return buffers;
}

IndexedBuffer
generateCubeOutlineBuffers() {
	IndexedBuffer buffers {
		createBuffer(),
		createBuffer(),
		createVertexArray(),
		0
	};

	// Cube vertices: 3 for vertex position.
	// Each face of the cube is made of 2 triangles, so 6 vertices per face
	unsigned int indices[] = {
		// Back face
		0, 1, 1, 3, 3, 2, 2,0,
		// Front face
		4, 5, 5, 7, 7, 6,6, 4,
		// Connecting lines
		0, 4, 1, 5, 2, 6, 3, 7,
	};


	GL_CHECK(glBindVertexArray(buffers.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo.get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo.get()));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	// Position attribute
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	// Unbind VAO
	GL_CHECK(glBindVertexArray(0));

	buffers.indexCount = 24;
	buffers.mode = GL_LINES;
	return buffers;
}


IndexedBuffer
generateCubeBuffers() {
	IndexedBuffer buffers {
		createBuffer(),
		createBuffer(),
		createVertexArray(),
		0
	};

	// Cube vertices: 3 for vertex position.
	// Each face of the cube is made of 2 triangles, so 6 vertices per face
	unsigned int indices[] = {
		// Back face
		0, 1, 3, 0, 3, 2,
		// Front face
		4, 5, 7, 4, 7, 6,
		// Left face
		0, 4, 6, 0, 6, 2,
		// Right face
		1, 5, 7, 1, 7, 3,
		// Bottom face
		0, 4, 5, 0, 5, 1,
		// Top face
		2, 6, 7, 2, 7, 3
	};


	GL_CHECK(glBindVertexArray(buffers.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo.get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo.get()));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	// Position attribute
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	// Unbind VAO
	GL_CHECK(glBindVertexArray(0));

	buffers.indexCount = 36;
	buffers.mode = GL_TRIANGLES;
	return buffers;
}


IndexedBuffer
generateCubeBuffersNormTex() {
	IndexedBuffer buffers {
		createBuffer(),
		createBuffer(),
		createVertexArray(),
		0
	};
	std::vector<VertexNormTex> vertices;
	std::vector<unsigned int> indices;
	for (int i = 0; i < 3; ++i) {
		for (int direction = -1; direction < 2; direction +=2) {
			unsigned indexOffset = unsigned(vertices.size());
			for (int j = 0; j < 4; ++j) {
				vertices.push_back(VertexNormTex(
					insertDimension(unitFaceVertices[j], i, direction * 0.5f),
					insertDimension(glm::vec2(), i, float(direction)),
					unitFaceVertices[j] + glm::vec2(0.5f, 0.5f)));
			}

			for (auto index : faceTriangleIndices) {
				indices.push_back(index + indexOffset);
			}
		}
	}

	GL_CHECK(glBindVertexArray(buffers.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo.get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormTex) * vertices.size(), vertices.data(), GL_STATIC_DRAW));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo.get()));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW));

	// Position attribute
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	// Normal attribute
	GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex), (void*)(sizeof(glm::vec3))));
	GL_CHECK(glEnableVertexAttribArray(1));

	// Texture coordinate attribute
	GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex), (void*)(2*sizeof(glm::vec3))));
	GL_CHECK(glEnableVertexAttribArray(2));

	// Unbind VAO
	GL_CHECK(glBindVertexArray(0));

	buffers.indexCount = unsigned(indices.size());
	buffers.mode = GL_TRIANGLES;
	return buffers;
}

IndexedBuffer
generateMeshBuffersNormTex(const ObjMesh &aMesh) {
	IndexedBuffer buffers {
		createBuffer(),
		createBuffer(),
		createVertexArray(),
		0
	};


	GL_CHECK(glBindVertexArray(buffers.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo.get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormTex) * aMesh.vertices.size(), aMesh.vertices.data(), GL_STATIC_DRAW));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo.get()));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * aMesh.indices.size(), aMesh.indices.data(), GL_STATIC_DRAW));

	// Position attribute
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	// Normal attribute
	GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex), (void*)(sizeof(glm::vec3))));
	GL_CHECK(glEnableVertexAttribArray(1));

	// Texture coordinate attribute
	GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex), (void*)(2*sizeof(glm::vec3))));
	GL_CHECK(glEnableVertexAttribArray(2));

	// Unbind VAO
	GL_CHECK(glBindVertexArray(0));

	buffers.indexCount = unsigned(aMesh.indices.size());
	buffers.mode = GL_TRIANGLES;
	return buffers;
};
