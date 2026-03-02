#pragma once


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "scene_object.hpp"

class Camera : public SceneObject {
public:
	Camera(float aspectRatio, float fov=45.0f, float nearPlane=0.01f, float farPlane=100.0f)
		: SceneObject(), fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {}

	void setAspectRatio(float aAspectRatio) {
		aspectRatio = aAspectRatio;
	}

	glm::mat4 getViewMatrix() const {
		// Convert quaternion rotation to rotation matrix
		glm::mat4 rotationMat = glm::inverse(glm::toMat4(rotation));
		glm::mat4 view = rotationMat * glm::translate(glm::mat4(1.0f), -position);
		return view;
	}

	glm::mat4 getProjectionMatrix() const {
		return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	}

	void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) {
		glm::vec3 direction = glm::normalize(target - position);
		// Create a look at quaternion
		rotation = glm::quatLookAt(direction, up);
	}

	void yaw(float angle) {
		glm::quat rotationQuat = glm::angleAxis(angle, getUpVector());
		rotation = rotationQuat * rotation;
	}

	void yawGlobal(float angle) {
		glm::quat rotationQuat = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
		rotation = rotationQuat * rotation;
	}

	void pitch(float angle) {
		glm::quat rotationQuat = glm::angleAxis(angle, getRightVector());
		rotation = rotationQuat * rotation;
	}

	void orbit(const glm::vec2 &aAngles, const glm::vec3 aOrigin) {
		{
			glm::vec3 axis = getUpVector();

			float angle = glm::radians(aAngles.x);
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
			glm::vec4 newPos = rotationMatrix * glm::vec4(position, 1.0);

			position = newPos.xyz();
			lookAt(aOrigin, getUpVector());
		}
		{
			glm::vec3 axis = getRightVector();

			float angle = glm::radians(aAngles.y);
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
			glm::vec4 newPos = rotationMatrix * glm::vec4(position, 1.0);

			position = newPos.xyz();
			lookAt(aOrigin, getUpVector());
		}
	}

	float near() const {
		return nearPlane;
	}

	float far() const {
		return farPlane;
	}

private:
	float fov;          // Field of view in radians
	float aspectRatio;  // Aspect ratio of the viewport
	float nearPlane;    // Near clipping plane
	float farPlane;     // Far clipping plane
};
