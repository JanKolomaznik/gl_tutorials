#include <iostream>
#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include "ogl_resource.hpp"
#include "error_handling.hpp"
#include "window.hpp"
#include "shader.hpp"

/////////////////////////////////////
// Uncomment just one of the following headers
// to get different variations of the demonstrations.

// #include "simpleTriangle.hpp"
// #include "coloredTriangle.hpp"
#include "fractal.hpp"

int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	try {
		auto window = Window();

		auto shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
		GLint seedLocation = glGetUniformLocation(shaderProgram.get(), "seed");

		// Set up vertex data and buffers and configure vertex attributes
		auto geometry = createGeometry();

		window.runLoop([&] {
			// Render
			GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
			GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

			GL_CHECK(glUseProgram(shaderProgram.get()));

			updateSeed(window.elapsedTime(), seedLocation);

			geometry.bindVertexArrays();
			GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, geometry.size));
		});
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



