#pragma once

#include <sstream>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "ogl_resource.hpp"
#include "error_handling.hpp"

namespace fs = std::filesystem;

inline std::string getShaderTypeName(GLenum aShaderType) {
	switch (aShaderType) {
	case GL_VERTEX_SHADER:
		return "Vertex Shader";
	case GL_FRAGMENT_SHADER:
		return "Fragment Shader";
	case GL_GEOMETRY_SHADER:
		return "Geometry Shader";
	#ifdef GL_COMPUTE_SHADER
	case GL_COMPUTE_SHADER:
		return "Compute Shader";
	#endif
	#ifdef GL_TESS_CONTROL_SHADER
	case GL_TESS_CONTROL_SHADER:
		return "Tessellation Control Shader";
	#endif
	#ifdef GL_TESS_EVALUATION_SHADER
	case GL_TESS_EVALUATION_SHADER:
		return "Tessellation Evaluation Shader";
	#endif
	default:
		return "Unknown Shader Type";
	}
}

inline std::string getGLTypeName(GLenum type) {
	switch (type) {
	case GL_FLOAT: return "float";
	case GL_FLOAT_VEC2: return "vec2";
	case GL_FLOAT_VEC3: return "vec3";
	case GL_FLOAT_VEC4: return "vec4";
	case GL_DOUBLE: return "double";
	case GL_INT: return "int";
	case GL_UNSIGNED_INT: return "unsigned int";
	case GL_BOOL: return "bool";
	case GL_FLOAT_MAT2: return "mat2";
	case GL_FLOAT_MAT3: return "mat3";
	case GL_FLOAT_MAT4: return "mat4";
	case GL_SAMPLER_2D: return "sampler2D";
	case GL_SAMPLER_3D: return "sampler3D";
	case GL_SAMPLER_CUBE: return "samplerCube";
	case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
	case GL_IMAGE_1D: return "image1D";
	case GL_IMAGE_2D: return "image2D";
	case GL_IMAGE_3D: return "image3D";
	// Add more types as needed
	default: return "Unknown Type " + std::to_string(type);
	}
}

struct UniformInfo {
	std::string name;
	GLenum type;
	GLint location;
};


class ShaderCompilationError: public OpenGLError {
public:
	ShaderCompilationError(const std::string& message, GLenum aShaderType)
		: OpenGLError(message)
		, mShaderType(aShaderType)
	{}

	std::string shaderTypeName() const {
		return getShaderTypeName(mShaderType);
	}

	GLenum shaderType() const {
		return mShaderType;
	}
protected:
	GLenum mShaderType;
};

inline std::string getShaderInfoLog(GLuint shader) {
	GLint logLength = 0;
	GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength));

	std::vector<char> log(logLength);
	if (logLength > 0) {
		GL_CHECK(glGetShaderInfoLog(shader, logLength, nullptr, log.data()));
		return std::string(log.begin(), log.end());
	}

	return {};
}

inline auto compileShader(GLenum aShaderType, const std::string& aSource) {
	auto shader = createShader(aShaderType);
	const char* src = aSource.c_str();
	GL_CHECK(glShaderSource(shader.get(), 1, &src, nullptr));
	GL_CHECK(glCompileShader(shader.get()));

	// Error handling
	int result;
	GL_CHECK(glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		throw ShaderCompilationError(getShaderInfoLog(shader.get()), aShaderType);
	}

	return shader;
}

using CompiledShaderStages = std::vector<const OpenGLResource *>;

inline auto createShaderProgram(const CompiledShaderStages &aShaderStages) {
	auto program = createShaderProgram();
	for (auto &shader : aShaderStages) {
		GL_CHECK(glAttachShader(program.get(), shader->get()));
	}
	GL_CHECK(glLinkProgram(program.get()));

	GLint isLinked = 0;
	GL_CHECK(glGetProgramiv(program.get(), GL_LINK_STATUS, &isLinked));
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		GL_CHECK(glGetProgramiv(program.get(), GL_INFO_LOG_LENGTH, &maxLength));

		std::vector<GLchar> infoLog(maxLength);
		GL_CHECK(glGetProgramInfoLog(program.get(), maxLength, &maxLength, &infoLog[0]));

		throw OpenGLError("Shader program linking failed:" + std::string(infoLog.begin(), infoLog.end()));
	}
	GL_CHECK(glValidateProgram(program.get()));

	GLint isValid = 0;
	GL_CHECK(glGetProgramiv(program.get(), GL_VALIDATE_STATUS, &isValid));
	if (isValid == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program.get(), GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program.get(), maxLength, &maxLength, &infoLog[0]);

		throw OpenGLError("Shader program validation failed:" + std::string(infoLog.begin(), infoLog.end()));
	}
	return program;
}

inline auto createShaderProgram(const OpenGLResource& vertexShader, OpenGLResource& fragmentShader) {
	return createShaderProgram(CompiledShaderStages{ &vertexShader, &fragmentShader });
}

inline auto createShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {
	auto vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	auto fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	return createShaderProgram(vs, fs);
}

inline auto createComputeShaderProgram(const std::string& computeShader) {
	auto cs = compileShader(GL_COMPUTE_SHADER, computeShader);

	return createShaderProgram(CompiledShaderStages{ &cs });
}

inline std::vector<UniformInfo> listShaderUniforms(const OpenGLResource &aShaderProgram) {
	std::vector<UniformInfo> uniforms;
	GLint numUniforms = 0;
	GL_CHECK(glGetProgramiv(aShaderProgram.get(), GL_ACTIVE_UNIFORMS, &numUniforms));

	std::vector<GLchar> nameData(256);
	for (GLint i = 0; i < numUniforms; ++i) {
		GLint arraySize = 0;
		GLenum type = 0;
		GLsizei actualLength = 0;
		GL_CHECK(glGetActiveUniform(aShaderProgram.get(), i, GLsizei(nameData.size()), &actualLength, &arraySize, &type, &nameData[0]));
		std::string name((char*)nameData.data(), actualLength);

		GLint location = glGetUniformLocation(aShaderProgram.get(), name.c_str());

		uniforms.emplace_back(name, type, location);
	}
	return uniforms;
}


inline std::string loadShaderSource(const fs::path& filePath) {
	// Check if the file exists before trying to open it
	if (!fs::exists(filePath)) {
		throw OpenGLError("File does not exist: " + filePath.string());
	}

	std::ifstream fileStream(filePath, std::ios::binary); // Open in binary mode to preserve all data

	if (!fileStream.is_open()) {
		throw std::runtime_error("Failed to open file: " + filePath.string());
	}

	// Use iterators to read the file content into a string
	std::string fileContent((std::istreambuf_iterator<char>(fileStream)),
			std::istreambuf_iterator<char>());

	return fileContent;
}

