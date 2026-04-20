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

static int const cWorkGroupSize = 256;

void compute() {
	auto timeQuery = createQuery();
	// Create compute shader
	auto scanLocalShaderSource = loadShaderSource("./shaders/scan_local.compute.glsl");
	auto scanLocalProgram = createComputeShaderProgram(scanLocalShaderSource);

	// auto scanSumShaderSource = loadShaderSource("./shaders/scan_sum.compute.glsl");
	// auto scanSumProgram = createComputeShaderProgram(scanSumShaderSource);

	// Create buffer
	std::vector<int> data(1024, 1);

	size_t numWorkGroups = (data.size() + cWorkGroupSize - 1) / cWorkGroupSize;  // Round up division
	size_t numElements = data.size();
	std::vector<int> aux(numWorkGroups, 0);

	// Generate and bind the data buffer
	auto dataBuffer = createBuffer();
	// glGenBuffers(1, &dataBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffer.get());
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(int), data.data(), GL_DYNAMIC_DRAW);

	// Generate and bind the auxiliary buffer
	// glGenBuffers(1, &auxBuffer);
	auto auxBuffer = createBuffer();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, auxBuffer.get());
	glBufferData(GL_SHADER_STORAGE_BUFFER, aux.size() * sizeof(int), aux.data(), GL_DYNAMIC_DRAW);

	// Optionally, unbind the buffer to avoid accidental modifications
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	auto start = std::chrono::high_resolution_clock::now();
	glBeginQuery(GL_TIME_ELAPSED, timeQuery.get());
	// Dispatch the local scan shader
	glUseProgram(scanLocalProgram.get());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dataBuffer.get());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, auxBuffer.get());
	glDispatchCompute((numElements + cWorkGroupSize -1) / cWorkGroupSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// *******************************************************
	// TODO - complete the sum
	// *******************************************************

	glEndQuery(GL_TIME_ELAPSED);
	auto end = std::chrono::high_resolution_clock::now();

	GLuint64 elapsedTime;
	glGetQueryObjectui64v(timeQuery.get(), GL_QUERY_RESULT, &elapsedTime);

	// Read back data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dataBuffer.get());
	int* ptr = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	std::vector<int> output(ptr, ptr + data.size());
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	//
	// Read back data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, auxBuffer.get());
	int* auxPtr = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	std::vector<int> auxOutput(auxPtr, auxPtr + aux.size());
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Print time taken
	std::chrono::duration<double, std::milli> elapsed = end - start;
	std::cout << "Compute shader time: " << elapsed.count() << " ms" << std::endl;
	std::cout << "Compute shader time (query): " << (elapsedTime / 1.0e6) << " ms" << std::endl;

	std::cout << "Result check: \n";
	for (auto i : output) {
		std::cout << i <<", ";
	}
	std::cout << "\n";
	std::cout << "\n";

	for (auto i : auxOutput) {
		std::cout << i <<", ";
	}
	std::cout << "\n";
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



