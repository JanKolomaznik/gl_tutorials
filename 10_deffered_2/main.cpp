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


#include "scene_definition.hpp"
#include "renderer.hpp"

#include "ogl_geometry_factory.hpp"
#include "ogl_material_factory.hpp"

#include <glm/gtx/string_cast.hpp>

void toggle(const std::string &aToggleName, bool &aToggleValue) {

	aToggleValue = !aToggleValue;
	std::cout << aToggleName << ": " << (aToggleValue ? "ON\n" : "OFF\n");
}

struct Config {
	int currentSceneIdx = 0;
	bool showSolid = true;
	bool showWireframe = false;
	bool useZOffset = false;
};

const float cMouseSensitivity = 0.2f;
const float cMovementSpeed = 2.0f;

int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	try {
		auto window = Window();
		MouseTracking mouseTracking;
		Config config;
		Camera camera(window.aspectRatio(), 45.0f, 0.5f, 1000.0f);
		camera.setPosition(glm::vec3(0.0f, 50.0f, 500.0f));
		camera.lookAt(glm::vec3(-100, 0, 520));
		SpotLight light(90, 100.0f, 2000.0f);
		light.setPosition(glm::vec3(600.0f, 950.0f, 700.0f));
		light.lookAt(glm::vec3());



		window.onCheckInput([&camera, &mouseTracking, &window](GLFWwindow *aWin) {
				auto winSize = window.size();
				mouseTracking.reset(winSize[0] / 2, winSize[1] / 2);
				mouseTracking.update(aWin);
				if (glfwGetMouseButton(aWin, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					// camera.orbit(-0.4f * mouseTracking.offset(), glm::vec3());

				}
				camera.yawGlobal(-cMouseSensitivity * glm::radians(mouseTracking.offset()[0]));
				camera.pitch(-cMouseSensitivity * glm::radians(mouseTracking.offset()[1]));
				window.setCursorToCenter();
				if (glfwGetKey(aWin, GLFW_KEY_W) == GLFW_PRESS) {
					camera.move(-cMovementSpeed * camera.getForwardVector());
				}
				if (glfwGetKey(aWin, GLFW_KEY_S) == GLFW_PRESS) {
					camera.move(cMovementSpeed * camera.getForwardVector());
				}
				if (glfwGetKey(aWin, GLFW_KEY_A) == GLFW_PRESS) {
					camera.move(-cMovementSpeed * camera.getRightVector());
				}
				if (glfwGetKey(aWin, GLFW_KEY_D) == GLFW_PRESS) {
					camera.move(cMovementSpeed * camera.getRightVector());
				}

			});
		window.setKeyCallback([&config, &camera](GLFWwindow *aWin, int key, int scancode, int action, int mods) {
				if (action == GLFW_PRESS) {
					switch (key) {
					case GLFW_KEY_ENTER:
						camera.setPosition(glm::vec3(0.0f, 50.0f, 500.0f));
						camera.lookAt(glm::vec3(-100, 0, 520));
						break;
					}
				}
			});

		OGLMaterialFactory materialFactory;
		materialFactory.loadShadersFromDir("./shaders/");
		materialFactory.loadTexturesFromDir("./data/textures/");

		OGLGeometryFactory geometryFactory;


		std::array<SimpleScene, 1> scenes {
			createIslandScene(materialFactory, geometryFactory),
		};

		Renderer renderer(materialFactory);
		window.onResize([&camera, &window, &renderer](int width, int height) {
				camera.setAspectRatio(window.aspectRatio());
				renderer.initialize(width, height);
			});


		renderer.initialize(window.size()[0], window.size()[1]);
		window.runLoop([&] {
			renderer.shadowMapPass(scenes[config.currentSceneIdx], light);
			// renderer.shadowMapPass(scenes[config.currentSceneIdx], camera);

			renderer.clear();
			renderer.geometryPass(scenes[config.currentSceneIdx], camera, light, RenderOptions{"solid"});
			renderer.compositingPass(light);
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
