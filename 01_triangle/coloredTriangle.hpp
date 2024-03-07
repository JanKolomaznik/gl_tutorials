#pragma once

#include <glad/glad.h>

// Shader sources
inline std::string vertexShaderSource = R"glsl(
	#version 410 core

	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aColor;

	out vec3 vertexColor; // Variable to transfer color to the fragment shader

	void main() {
		gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
		vertexColor = aColor; // Pass color to the fragment shader
	}
)glsl";

inline std::string fragmentShaderSource = R"glsl(
	#version 410 core

	in vec3 vertexColor; // Color from the vertex shader

	out vec4 fragColor;

	void main() {
		fragColor = vec4(vertexColor, 1.0);
	}
)glsl";


struct Vertex {
    // Position data: x, y, z
    float position[3];
    // Color data: r, g, b
    float color[3];
};


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

	std::vector<Vertex> vertices{
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // Vertex 1: position and color (red)
		{{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Vertex 2: position and color (green)
		{{ 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}  // Vertex 3: position and color (blue)
	};

	GL_CHECK(glBindVertexArray(geometry.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo.get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW));

	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float))));
	GL_CHECK(glEnableVertexAttribArray(1));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	geometry.size = vertices.size();
	return geometry;
}


struct Geometry2 {
	OpenGLResource vao;
	OpenGLResource vbo;
	OpenGLResource vboCol;

	int size = 0;

	void bindVertexArrays() {
		glBindVertexArray(vao.get());
	}
};

inline Geometry2 createGeometry2() {
	Geometry2 geometry{ createVertexArray(), createBuffer(), createBuffer() };

	std::vector<float> vertices{
		-0.5f, -0.5f, 0.0f,// Vertex 1: position
		 0.5f, -0.5f, 0.0f,// Vertex 2: position
		 0.0f,  0.5f, 0.0f,// Vertex 3: position
	};

	std::vector<float> colors{
		1.0f, 0.0f, 0.0f, // Vertex 1: color (red)
		0.0f, 1.0f, 0.0f, // Vertex 2: color (green)
		0.0f, 0.0f, 1.0f  // Vertex 3: color (blue)
	};

	glBindVertexArray(geometry.vao.get());

	glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo.get());
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, geometry.vboCol.get());
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	geometry.size = vertices.size();
	return geometry;
}
