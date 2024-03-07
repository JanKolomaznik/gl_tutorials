#pragma once

#include <exception>
#include <iostream>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
	Window() {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Create a windowed mode window and its OpenGL context
		mWindow = glfwCreateWindow(800, 600, "OpenGL Example", NULL, NULL);
		if (!mWindow) {
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);

		makeCurrent();
		// Load OpenGL functions using glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw std::runtime_error("Failed to initialize GLAD");
		}
	}

	~Window() {

	}

	void makeCurrent() {
		glfwMakeContextCurrent(mWindow);
	}

	void runLoop(std::function<void(void)> loopBody) {
		while (!glfwWindowShouldClose(mWindow)) {
			processInput(mWindow);

			loopBody();

			// GLFW: swap buffers and poll IO events
			glfwSwapBuffers(mWindow);
			glfwPollEvents();
		}
	}

	double elapsedTime() const {
		return glfwGetTime();
	}

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		GL_CHECK(glViewport(0, 0, width, height));
	}

	void processInput(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
	}

protected:
	GLFWwindow* mWindow;
};
