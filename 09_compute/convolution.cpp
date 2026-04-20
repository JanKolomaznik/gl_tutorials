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
#include "ogl_material_factory.hpp"

#include <glm/gtx/string_cast.hpp>

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

static int const cWorkGroupSize = 256;

void saveTextureToFile(GLuint textureId, int texWidth, int texHeight, const char* filename) {
    glBindTexture(GL_TEXTURE_2D, textureId);

    GLubyte* imageData = new GLubyte[texWidth * texHeight * 3]; // Assuming RGB
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_write_png(filename, texWidth, texHeight, 3, imageData, texWidth * 3);
    delete[] imageData;
}


void compute() {
	auto timeQuery = createQuery();
	// Create compute shader
	auto convolutionShaderSource = loadShaderSource("./shaders/convolution.compute.glsl");
	auto convolutionProgram = createComputeShaderProgram(convolutionShaderSource);

	auto imageData = loadImage("./data/textures/test2.jpg");

	int width, height;
	width = imageData->width;
	height = imageData->height;
	std::cout << width << "\n";
	std::cout << height << "\n";

	auto inputTexture = createTexture();
	glBindTexture(GL_TEXTURE_2D, inputTexture.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData->data.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	auto outputTexture = createTexture();
	glBindTexture(GL_TEXTURE_2D, outputTexture.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);


	GL_CHECK(glUseProgram(convolutionProgram.get()));
	GL_CHECK(glBindImageTexture(0, inputTexture.get(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F));
	GL_CHECK(glBindImageTexture(1, outputTexture.get(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F));

	auto start = std::chrono::high_resolution_clock::now();
	glBeginQuery(GL_TIME_ELAPSED, timeQuery.get());
	GL_CHECK(glDispatchCompute((width + 15) / 16, (height + 15) / 16, 1));
	GL_CHECK(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
	glEndQuery(GL_TIME_ELAPSED);
	auto end = std::chrono::high_resolution_clock::now();

	GLuint64 elapsedTime;
	glGetQueryObjectui64v(timeQuery.get(), GL_QUERY_RESULT, &elapsedTime);

	saveTextureToFile(outputTexture.get(), width, height, "out.png");
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
