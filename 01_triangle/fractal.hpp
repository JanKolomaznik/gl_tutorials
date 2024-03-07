#pragma once

#include <glad/glad.h>
#include <cmath>

// Shader sources
inline std::string vertexShaderSource = R"glsl(
	#version 410 core
	layout (location = 0) in vec2 aPos;

	out vec2 texCoord;

	void main() {
		gl_Position = vec4(aPos, 0.0, 1.0);
		texCoord = aPos;
	}
)glsl";

inline std::string fragmentShaderSource = R"glsl(
	#version 410 core

	vec4 randomColor(int seed) {
		float r = ((seed * 315 + 714) % 255) / 255.0;
		float g = ((seed * 861 + 346) % 255) / 255.0;
		float b = ((seed * 123 + 1068) % 255) / 255.0;
		return vec4(r, g, b, 1.0);
	}

	in vec2 texCoord;

	out vec4 fragColor;

	uniform vec2 seed = vec2(-0.8, 0.156);

	const int maxIter = 100;
	void main() {
		int i = 0;
		vec2 z = vec2(3.0 * texCoord.x, 2.0 * texCoord.y);

		for (; i < maxIter; ++i) {
			float x = (z.x * z.x - z.y * z.y) + seed.x;
			float y = (z.y * z.x + z.x * z.y) + seed.y;

			if ((x * x + y * y) > 4.0) {
			    break;
			}

			z.x = x;
			z.y = y;
		}
		fragColor = randomColor(i);
	}
)glsl";


struct Geometry {
	OpenGLResource vao;
	OpenGLResource vbo;

	int size = 0;

	void bindVertexArrays() {
		GL_CHECK(glBindVertexArray(vao.get()));
	}
};

inline Geometry createGeometry() {
	Geometry geometry{ createVertexArray(), createBuffer() };

	std::vector<float> vertices{
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		 1.0f,  1.0f,

		 1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f, -1.0f,
	};

	GL_CHECK(glBindVertexArray(geometry.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo.get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW));

	GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	geometry.size = vertices.size() / 2;
	return geometry;
}

inline double floatmod(double a, double b) {
	auto d = a / b;
	return (d - round(d)) * b;
}

inline void updateSeed(double time, GLuint seedLocation) {
	float v1 = float(floatmod(time / 3.346, 2.00) - 1.0);
	float v2 = float(floatmod(time / 7.2135, 2.00) - 1.0);
	GL_CHECK(glUniform2f(seedLocation, v1, v2));
}
