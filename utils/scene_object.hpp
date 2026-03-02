#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp> // For glm::quat
#include <glm/gtx/string_cast.hpp>

#include "material_factory.hpp"
#include "geometry_factory.hpp"

struct RenderData {
	const glm::mat4 modelMat;
	const MaterialParameters &mMaterialParams;
	const AShaderProgram &mShaderProgram;
	const AGeometry &mGeometry;
};

struct RenderOptions {
	std::string mode;
};

struct RenderInfo {
	MaterialParameters materialParams;
	std::shared_ptr<AShaderProgram> shaderProgram;
	std::shared_ptr<AGeometry> geometry;
};

class SceneObject {
public:
	SceneObject()
		: position(glm::vec3(0.0f)),
		rotation(glm::quat(glm::vec3(0.0f))), // Initialize with an identity quaternion
		scale(glm::vec3(1.0f)) {}

	virtual ~SceneObject() {}

	// Setters
	void setPosition(const glm::vec3& pos) { position = pos; }
	void setRotation(const glm::quat& rot) { rotation = rot; }
	void setScale(const glm::vec3& scl) { scale = scl; }
	void setName(const std::string &aName) {
		mName = aName;
	}

	void move(const glm::vec3& movement) { position += movement; }

	// Getters
	const glm::vec3& getPosition() const { return position; }
	const glm::quat& getRotation() const { return rotation; }
	const glm::vec3& getScale() const { return scale; }
	const std::string& getName() const { return mName; }

	glm::mat4 getModelMatrix() const {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model *= glm::toMat4(rotation);
		model = glm::scale(model, scale);
		return model;
	}

	// Rendering interface
	virtual std::optional<RenderData> getRenderData(const RenderOptions &aOptions) const {
		return std::optional<RenderData>();
	}

	virtual void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {};


	glm::vec3 getForwardVector() const{
		return rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	}

	glm::vec3 getUpVector() const {
		return rotation * glm::vec3(0.0f, 1.0f, 0.0f);
	}

	glm::vec3 getRightVector() const {
		return rotation * glm::vec3(1.0f, 0.0f, 0.0f);
	}

	void printInfo(std::ostream &aStream) const {
		aStream
			<< "Position: " << glm::to_string(position) << "\n"
			<< "Rotation: " << glm::to_string(rotation) << "\n"
			<< "Rotation (Euler): " << glm::to_string(glm::degrees(glm::eulerAngles(rotation))) << "\n"
			<< "Scale: " << glm::to_string(scale) << "\n"
			<< "Up: " << glm::to_string(getUpVector()) << "\n"
			<< "Right: " << glm::to_string(getRightVector()) << "\n"
			<< "Forward: " << glm::to_string(getForwardVector()) << "\n";
	}

protected:
	glm::vec3 position;
	glm::quat rotation; // Using quaternion for rotation
	glm::vec3 scale;

	std::string mName;
};


class AxisGizmo: public SceneObject {
public:
	std::optional<RenderData> getRenderData(const RenderOptions &aOptions) const {
		return std::optional<RenderData>(RenderData{
				getModelMatrix(),
				mRenderInfo.materialParams,
				*(mRenderInfo.shaderProgram),
				*(mRenderInfo.geometry)
			});
	}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
		mRenderInfo.shaderProgram = aMaterialFactory.getShaderProgram("linegizmo");
		mRenderInfo.geometry = aGeometryFactory.getAxisGizmo();
	};

	RenderInfo mRenderInfo;
};
