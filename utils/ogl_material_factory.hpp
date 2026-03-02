#pragma once

#include <filesystem>
#include <regex>
#include <map>
#include <variant>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "shader.hpp"
#include "material_factory.hpp"

namespace fs = std::filesystem;

using ShaderFiles = std::map<std::string, fs::path>;
using ShaderProgramFiles = std::map<std::string, ShaderFiles>;

class OGLTexture: public ATexture {
public:
	OGLTexture(
		OpenGLResource &&aTexture,
		GLenum aTextureKind = GL_TEXTURE_2D)
		: texture(std::move(aTexture))
		, textureKind(aTextureKind)
	{}
	OpenGLResource texture;
	GLenum textureKind;
};

template<class>
inline constexpr bool always_false_v = false;

inline int setUniform(const UniformInfo &aInfo, const MaterialParam &aParam, int aNextTexturingUnit) {
	std::visit([&aInfo, &aNextTexturingUnit](auto&& arg) {
		try {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, int>) {
				GL_CHECK(glUniform1i(aInfo.location, arg));
			} else 	if constexpr (std::is_same_v<T, unsigned int>) {
				GL_CHECK(glUniform1ui(aInfo.location, arg));
			} else if constexpr (std::is_same_v<T, float>) {
				GL_CHECK(glUniform1f(aInfo.location, arg));
			} else if constexpr (std::is_same_v<T, glm::vec2>) {
				GL_CHECK(glUniform2fv(aInfo.location, 1, glm::value_ptr(arg)));
			} else if constexpr (std::is_same_v<T, glm::vec3>) {
				GL_CHECK(glUniform3fv(aInfo.location, 1, glm::value_ptr(arg)));
			} else if constexpr (std::is_same_v<T, glm::vec4>) {
				GL_CHECK(glUniform4fv(aInfo.location, 1, glm::value_ptr(arg)));
			} else if constexpr (std::is_same_v<T, glm::mat3>) {
				GL_CHECK(glUniformMatrix3fv(aInfo.location, 1, GL_FALSE, glm::value_ptr(arg)));
			} else if constexpr (std::is_same_v<T, glm::mat4>) {
				GL_CHECK(glUniformMatrix4fv(aInfo.location, 1, GL_FALSE, glm::value_ptr(arg)));
			} else if constexpr (std::is_same_v<T, TextureInfo>) {
				if (arg.textureData) {
					GL_CHECK(glActiveTexture(GL_TEXTURE0 + aNextTexturingUnit));
					const OGLTexture &texture = static_cast<const OGLTexture &>(*arg.textureData);

					GL_CHECK(glBindTexture(texture.textureKind, texture.texture.get()));
					GL_CHECK(glUniform1i(aInfo.location, aNextTexturingUnit));
					++aNextTexturingUnit;
				}
			} else if constexpr (std::is_same_v<T, ArrayDescription>) {
				GL_CHECK(glUniform1fv(aInfo.location, arg.count, arg.ptr));
			} else {
				static_assert(always_false_v<T>, "non-exhaustive visitor!");
			}
		} catch (OpenGLError &exc) {
			std::cerr << "Error when setting uniform " << aInfo.name << "\n";
			throw;
		}
		},
		aParam);
	return aNextTexturingUnit;
}

class OGLShaderProgram: public AShaderProgram {
public:
	OGLShaderProgram(
		OpenGLResource &&aProgram,
		std::vector<UniformInfo> aUniforms
		)
		: program(std::move(aProgram))
		, uniforms(std::move(aUniforms))
	{}
	OpenGLResource program;
	std::vector<UniformInfo> uniforms;

	void use() const { GL_CHECK(glUseProgram(program.get())); }
	void setMaterialParameters(
		const MaterialParameterValues &aParameters
		) const
	{
		setMaterialParameters(aParameters, MaterialParameterValues{});
	}
	void setMaterialParameters(
		const MaterialParameterValues &aParameters,
		const MaterialParameterValues & aFallback) const
	{
		int nextTexturingUnit = 0;
		for (auto &uniform : uniforms) {
			auto it = aParameters.find(uniform.name);
			if (it == aParameters.end()) {
				it = aFallback.find(uniform.name);
				if (it == aFallback.end()) {
					// No value for uniform - skip setting
					continue;
				}
			}
			nextTexturingUnit = setUniform(uniform, it->second, nextTexturingUnit);
		}

	}
};


class OGLMaterialFactory: public MaterialFactory {
public:
	void loadShadersFromDir(fs::path aShaderDir);
	void loadTexturesFromDir(fs::path aTextureDir);
	void load3DTexturesFromDir(fs::path aTextureDir);

	std::shared_ptr<AShaderProgram> getShaderProgram(const std::string &aName) {
		auto it = mPrograms.find(aName);
		if (it == mPrograms.end()) {
			throw OpenGLError("Shader program " + aName + " not found");
		}

		return it->second;
	};

	std::shared_ptr<ATexture> getTexture(const std::string &aName) {
		auto it = mTextures.find(convertToIdentifier(aName));
		if (it == mTextures.end()) {
			throw OpenGLError("Texture " + aName + " not found");
		}

		return it->second;
	};

protected:
	using CompiledPrograms = std::map<std::string, std::shared_ptr<OGLShaderProgram>>;
	using Textures = std::map<std::string, std::shared_ptr<OGLTexture>>;

	CompiledPrograms mPrograms;
	Textures mTextures;
};

struct ImageData {
	std::unique_ptr<unsigned char, void(*)(void*)> data;
	int width, height, channels;

	ImageData(unsigned char* data, int width, int height, int channels);
};


std::unique_ptr<ImageData> loadImage(const fs::path& filePath);
OpenGLResource createTextureFromData(const ImageData& imgData);
