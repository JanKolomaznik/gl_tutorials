#include <iostream>
#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <array>

#include "ogl_resource.hpp"
#include "error_handling.hpp"
#include "window.hpp"
#include "shader.hpp"

// #include "scene_definition.hpp"
// #include "renderer.hpp"

#include <vector>
#include <chrono>

#include "ogl_geometry_factory.hpp"
#include "material_factory.hpp"

#include <glm/gtx/string_cast.hpp>

void compute() {

	auto timeQuery = createQuery();

	// Create compute shader
	auto shaderSource = loadShaderSource("./shaders/multiply.compute.glsl");
	auto shaderProgram = createComputeShaderProgram(shaderSource);
	// Create buffer
	std::vector<float> data(1024, 1.0f); // 1024 elements initialized to 1.0
	auto buffer = createBuffer();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer.get());
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer.get());

	// Set uniform
	glUseProgram(shaderProgram.get());
	glUniform1f(glGetUniformLocation(shaderProgram.get(), "multiplier"), 2.0f);

	// Dispatch compute shader
	auto start = std::chrono::high_resolution_clock::now();
	glBeginQuery(GL_TIME_ELAPSED, timeQuery.get());
	glDispatchCompute((data.size() + 255) / 256, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glEndQuery(GL_TIME_ELAPSED);
	auto end = std::chrono::high_resolution_clock::now();

	GLuint64 elapsedTime;
	glGetQueryObjectui64v(timeQuery.get(), GL_QUERY_RESULT, &elapsedTime);

	// Read back data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer.get());
	float* ptr = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	std::vector<float> output(ptr, ptr + data.size());
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Print time taken
	std::chrono::duration<double, std::milli> elapsed = end - start;
	std::cout << "Compute shader time: " << elapsed.count() << " ms" << std::endl;
	std::cout << "Compute shader time (query): " << (elapsedTime / 1.0e6) << " ms" << std::endl;
}

int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	try {
		auto window = Window();

		compute();
	} catch (ShaderCompilationError &exc) {
		std::cerr
			<< "Shader compilation error!\n"
			<< "Shader type: " << exc.shaderTypeName()
			<<"\nError: " << exc.what() << "\n";
		return -3;
	} catch (OpenGLError &exc) {
		std::cerr << "OpenGL error: " << exc.what() << "\n";
		return -2;
	} catch (std::exception &exc) {
		std::cerr << "Error: " << exc.what() << "\n";
		return -1;
	}

	glfwTerminate();
	return 0;
}



