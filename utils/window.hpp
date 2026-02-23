#pragma once

#include <exception>
#include <iostream>
#include <functional>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

/**
 * @brief Manages a GLFW window and related callbacks.
 */
class Window {
public:
	/**
	 * @brief Constructor with configurable size, title, and OpenGL version.
	 *        By default, sets up a 800x600 window titled "OpenGL Example" with OpenGL 4.4 core profile.
	 *
	 * @param width         The desired window width.
	 * @param height        The desired window height.
	 * @param title         The window title.
	 * @param majorVersion  The desired OpenGL major version.
	 * @param minorVersion  The desired OpenGL minor version.
	 */
	Window(int width = 800,
		int height = 600,
		const std::string& title = "OpenGL Example",
		int majorVersion = 4,
		int minorVersion = 4)
	{
		initGLFWWindow(width, height, title, majorVersion, minorVersion);
		makeCurrent();
		initGLAD();
		glfwSetKeyCallback(mWindow, keyCallback);
		glfwSwapInterval(1);
	}

	/**
	 * @brief Cleans up the GLFW window if it exists.
	 */
	~Window() {
		if (mWindow) {
			glfwDestroyWindow(mWindow);
			mWindow = nullptr;
		}
		// Note: Generally, you call glfwTerminate() once in your entire application,
		//	   not necessarily in the destructor for a single window.
	}

	/**
	 * @return The current width and height of the window.
	 */
	std::array<int, 2> size() {
		std::array<int, 2> result{0, 0};
		glfwGetWindowSize(mWindow, &result[0], &result[1]);
		return result;
	}

	/**
	 * @brief Makes this window's context current on the calling thread.
	 */
	void makeCurrent() {
		glfwMakeContextCurrent(mWindow);
	}

	/**
	 * @brief Runs the main loop until the window should close.
	 *
	 * @param loopBody   Function to execute every loop iteration.
	 * @param pollEvents If true, automatically polls input/events each frame.
	 *				   If false, user must manually poll or use another mechanism.
	 */
	void runLoop(std::function<void(void)> loopBody, bool pollEvents = true) {
		while (!glfwWindowShouldClose(mWindow)) {
			if (pollEvents) {
				processInput(mWindow);
			}
			loopBody();
			glfwSwapBuffers(mWindow);
		}
	}

	/**
	 * @brief Registers a callback for when the framebuffer is resized.
	 * @param aOnResizeCallback A function taking width and height parameters.
	 */
	void onResize(std::function<void(int, int)> aOnResizeCallback) {
		mOnResizeCallback = aOnResizeCallback;
	}

	/**
	 * @brief Registers a callback to process input events each frame.
	 * @param aCheckInput A function taking a GLFWwindow pointer for custom input logic.
	 */
	void onCheckInput(std::function<void(GLFWwindow*)> aCheckInput) {
		mCheckInput = aCheckInput;
	}

	/**
	 * @brief Global key callback passed to GLFW, forwarding events to the Window instance if set.
	 */
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		void* ptr = glfwGetWindowUserPointer(window);
		if (ptr) {
			Window* winPtr = reinterpret_cast<Window*>(ptr);
			if (winPtr->mKeyCallback) {
				winPtr->mKeyCallback(window, key, scancode, action, mods);
			}
		}
	}

	/**
	 * @brief Registers a custom key callback.
	 * @param aKeyCallback Function matching GLFW key-callback signature.
	 */
	void setKeyCallback(std::function<void(GLFWwindow*, int, int, int, int)> aKeyCallback) {
		mKeyCallback = aKeyCallback;
	}

	/**
	 * @return Elapsed time in seconds since GLFW was initialized.
	 */
	double elapsedTime() const {
		return glfwGetTime();
	}

	/**
	 * @brief Callback for updating OpenGL viewport and forwarding resize event.
	 */
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		GL_CHECK(glViewport(0, 0, width, height));
		void* ptr = glfwGetWindowUserPointer(window);
		if (ptr) {
			Window* winPtr = reinterpret_cast<Window*>(ptr);
			if (winPtr->mOnResizeCallback) {
				winPtr->mOnResizeCallback(width, height);
			}
		}
	}

	/**
	 * @brief Processes keyboard/mouse input and polls events by default.
	 */
	void processInput(GLFWwindow* window) {
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		if (mCheckInput) {
			mCheckInput(mWindow);
		}
	}

	/**
	 * @return The aspect ratio of the window (width / height), or 0 if height is 0.
	 */
	float aspectRatio() const {
		int width, height;
		glfwGetWindowSize(mWindow, &width, &height);
		if (height == 0) {
			return 0.0f;
		}
		return static_cast<float>(width) / static_cast<float>(height);
	}

	/**
	 * @brief Moves the cursor to the center of the window.
	 */
	void setCursorToCenter() {
		auto winSize = size();
		glfwSetCursorPos(mWindow, winSize[0] / 2, winSize[1] / 2);
	}

protected:
	GLFWwindow* mWindow = nullptr;

	std::function<void(int, int)> mOnResizeCallback;
	std::function<void(GLFWwindow*, int, int, int, int)> mKeyCallback;
	std::function<void(GLFWwindow*)> mCheckInput;

private:
	/**
	 * @brief Initializes the GLFW window with specified parameters.
	 */
	void initGLFWWindow(int width, int height, const std::string& title,
						int majorVersion, int minorVersion)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		if (!mWindow) {
			throw std::runtime_error("Failed to create GLFW window");
		}
		glfwSetWindowUserPointer(mWindow, this);
		glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
	}

	/**
	 * @brief Loads OpenGL function pointers using GLAD.
	 */
	void initGLAD() {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw std::runtime_error("Failed to initialize GLAD");
		}
	}
};

/**
 * @brief Holds cursor positions and calculates movement offsets.
 */
struct MouseTracking {
	/**
	 * @brief Updates the current and previous cursor positions using glfwGetCursorPos.
	 */
	void update(GLFWwindow* window) {
		previousX = currentX;
		previousY = currentY;
		glfwGetCursorPos(window, &currentX, &currentY);
	}

	/**
	 * @brief Resets both current and previous positions to the given coordinates.
	 */
	void reset(float x, float y) {
		currentX = x;
		currentY = y;
		previousX = x;
		previousY = y;
	}

	/**
	 * @brief Resets positions to (0, 0).
	 */
	void reset() {
		reset(0.0f, 0.0f);
	}

	/**
	 * @return The offset (delta x, delta y) between the current and previous positions.
	 */
	glm::vec2 offset() {
		return glm::vec2(
			static_cast<float>(currentX - previousX),
			static_cast<float>(currentY - previousY)
		);
	}

	double currentX = 0.0;
	double currentY = 0.0;
	double previousX = 0.0;
	double previousY = 0.0;
};
