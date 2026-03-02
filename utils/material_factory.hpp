#pragma once

#include <filesystem>
#include <regex>
#include <map>
#include <variant>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace fs = std::filesystem;

class ATexture {
public:
	ATexture() {}
	virtual ~ATexture() {}
};

struct TextureInfo {
	std::string name;
	std::shared_ptr<ATexture> textureData;
};

struct ArrayDescription {
	int count = 0;
	const float *ptr = nullptr;
};

using MaterialParam = std::variant<
				int,
				unsigned int,
				float,
				glm::vec2,
				glm::vec3,
				glm::vec4,
				glm::mat3,
				glm::mat4,
				TextureInfo,
				ArrayDescription
				>;
using MaterialParameterValues = std::map<std::string, MaterialParam>;

enum class RenderStyle {
	Solid,
	Wireframe
};

class MaterialParameters {
public:
	MaterialParameters()
		: mRenderStyle(RenderStyle::Solid)
		, mIsTesselation(false)
	{}
	MaterialParameters(const std::string &aMaterialName, RenderStyle aRenderStyle, const MaterialParameterValues &aParameterValues, bool aIsTesselation = false)
		: mMaterialName(aMaterialName)
		, mRenderStyle(aRenderStyle)
		, mParameterValues(aParameterValues)
		, mIsTesselation(aIsTesselation)
	{}

	std::string mMaterialName;
	RenderStyle mRenderStyle;
	MaterialParameterValues mParameterValues;
	bool mIsTesselation;
};

class AShaderProgram {
public:
	AShaderProgram() {}
	virtual ~AShaderProgram() {}
};

inline std::string convertToIdentifier(std::string aId) {
	std::replace(aId.begin(), aId.end(), '\\', '/');
	return aId;
}

class MaterialFactory {
public:
	virtual std::shared_ptr<AShaderProgram> getShaderProgram(const std::string &aName) = 0;
	virtual std::shared_ptr<ATexture> getTexture(const std::string &aName) = 0;
};
