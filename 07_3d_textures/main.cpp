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
	bool showRaycasted = true;
};

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
		Camera camera(window.aspectRatio());
		camera.setPosition(glm::vec3(0.0f,0.0f, -3.0f));
		camera.lookAt(glm::vec3());
		window.onResize([&camera, &window](int width, int height) {
				camera.setAspectRatio(window.aspectRatio());
			});

		window.onCheckInput([&camera, &mouseTracking](GLFWwindow *aWin) {
				mouseTracking.update(aWin);
				if (glfwGetMouseButton(aWin, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					camera.orbit(-0.4f * mouseTracking.offset(), glm::vec3());
				}
			});
		window.setKeyCallback([&config, &camera](GLFWwindow *aWin, int key, int scancode, int action, int mods) {
				if (action == GLFW_PRESS) {
					switch (key) {
					case GLFW_KEY_ENTER:
						camera.setPosition(glm::vec3(0.0f,0.0f, -3.0f));
						camera.lookAt(glm::vec3());
						break;
					case GLFW_KEY_1:
						config.currentSceneIdx = 0;
						break;
					case GLFW_KEY_2:
						config.currentSceneIdx = 1;
						break;
					case GLFW_KEY_3:
						config.currentSceneIdx = 2;
						break;
					case GLFW_KEY_4:
						config.currentSceneIdx = 3;
						break;
					case GLFW_KEY_W:
						toggle("Show wireframe", config.showWireframe);
						break;
					case GLFW_KEY_S:
						toggle("Show solid", config.showSolid);
						break;
					case GLFW_KEY_R:
						toggle("Show raycasted", config.showRaycasted);
						break;
					}
				}
			});

		OGLMaterialFactory materialFactory;
		materialFactory.loadShadersFromDir("./shaders/");
		// materialFactory.loadTexturesFromDir("./textures/");
		materialFactory.load3DTexturesFromDir("./3d_textures/");

		OGLGeometryFactory geometryFactory;


		std::array<SimpleScene, 4> scenes {
			createSphereScene(materialFactory, geometryFactory),
			createAnimationScene(materialFactory, geometryFactory),
			createMIPScene1(materialFactory, geometryFactory),
			createMIPScene2(materialFactory, geometryFactory),
		};

		Renderer renderer(materialFactory);

		renderer.initialize();
		window.runLoop([&] {
			renderer.setCurrentTime(float(window.elapsedTime()));
			renderer.clear();
			if (config.showSolid) {
				GL_CHECK(glDisable(GL_POLYGON_OFFSET_LINE));
				GL_CHECK(glPolygonOffset(0.0f, 0.0f));
				GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
				renderer.renderScene(scenes[config.currentSceneIdx], camera, RenderOptions{"solid"});
			}
			if (config.showRaycasted) {
				GL_CHECK(glDisable(GL_POLYGON_OFFSET_LINE));
				GL_CHECK(glPolygonOffset(0.0f, 0.0f));
				GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
				renderer.renderScene(scenes[config.currentSceneIdx], camera, RenderOptions{"raycast"});
			}
			if (config.showWireframe) {
				GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
				GL_CHECK(glEnable(GL_POLYGON_OFFSET_LINE));
				GL_CHECK(glPolygonOffset(-1.0f, -1.0f));
				renderer.renderScene(scenes[config.currentSceneIdx], camera, RenderOptions{"wireframe"});
			}
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



