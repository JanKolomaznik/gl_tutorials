#pragma once

#include <glad/glad.h>

// Shader sources
inline std::string vertexShaderSource = R"glsl(
	#version 410 core

	layout (location = 0) in vec3 aPos;

	void main() {
		gl_Position = vec4(aPos, 1.0);
	}
)glsl";

inline std::string fragmentShaderSource = R"glsl(
	#version 410 core

	out vec4 fragColor;

	void main() {
		fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
)glsl";


struct Geometry {
	OpenGLResource vao;
	OpenGLResource vbo;

	int size = 0;

	void bindVertexArrays() {
		glBindVertexArray(vao.get());
	}
};

inline Geometry createGeometry() {
	Geometry geometry{ createVertexArray(), createBuffer() };

	std::vector<float> vertices{
		-0.5f, -0.5f, 0.0f, // left
		 0.5f, -0.5f, 0.0f, // right
		 0.0f,  0.5f, 0.0f  // top
	};

	GL_CHECK(glBindVertexArray(geometry.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo.get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW));

	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	geometry.size = vertices.size() / 3;
	return geometry;
}
