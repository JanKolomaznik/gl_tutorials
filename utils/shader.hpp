#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ogl_resource.hpp"
#include "error_handling.hpp"

namespace fs = std::filesystem;

/**
 * @brief Converts an OpenGL shader stage enum to a human-readable name.
 *
 * Intended primarily for diagnostics (error messages, logging, UI).
 *
 * @param aShaderType Shader stage enum (e.g. GL_VERTEX_SHADER).
 * @return Readable shader stage name; "Unknown Shader Type" for unhandled values.
 */
[[nodiscard]] inline std::string getShaderTypeName(GLenum aShaderType) {
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

/**
 * @brief Converts a GLSL/OpenGL type enum (as reported by reflection) to a readable name.
 *
 * This is useful for printing uniform and attribute information returned by reflection
 * APIs such as glGetActiveUniform.
 *
 * @param type OpenGL type enum (e.g. GL_FLOAT_VEC3).
 * @return A short GLSL-like type string, or "Unknown Type <value>" for unhandled values.
 */
[[nodiscard]] inline std::string getGLTypeName(GLenum type) {
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

/**
 * @brief Reflection metadata for a single active uniform.
 *
 * @note If the uniform is optimized out by the compiler, it may not appear in the
 *       active uniform list, and/or its location may be -1.
 */
struct UniformInfo {
	std::string name;  ///< Uniform name as reported by glGetActiveUniform.
	GLenum type;       ///< Uniform type enum (e.g. GL_FLOAT_VEC4).
	GLint location;    ///< Location as returned by glGetUniformLocation (may be -1).
};

/**
 * @brief Exception type thrown when a shader fails to compile.
 *
 * Extends OpenGLError with shader-stage context.
 */
class ShaderCompilationError: public OpenGLError {
public:
	/**
	 * @brief Constructs a compilation error for a given shader stage.
	 *
	 * @param message Diagnostic message (typically the GLSL compiler log).
	 * @param aShaderType Shader stage enum.
	 */
	ShaderCompilationError(const std::string& message, GLenum aShaderType)
		: OpenGLError(message)
		, mShaderType(aShaderType)
	{}

	/**
	 * @brief Returns a human-readable shader stage name for this error.
	 *
	 * @return Stage name such as "Vertex Shader".
	 */
	[[nodiscard]] std::string shaderTypeName() const {
		return getShaderTypeName(mShaderType);
	}

	/**
	 * @brief Returns the OpenGL shader stage enum associated with this error.
	 */
	[[nodiscard]] GLenum shaderType() const {
		return mShaderType;
	}
protected:
	GLenum mShaderType; ///< Shader stage associated with the compilation failure.
};

/**
 * @brief Retrieves the OpenGL shader compiler log for a given shader object.
 *
 * @param shader OpenGL shader object ID.
 * @return The info log string, or an empty string if no log is available.
 */
[[nodiscard]] inline std::string getShaderInfoLog(GLuint shader) {
	GLint logLength = 0;
	GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength));

	std::vector<char> log(logLength);
	if (logLength > 0) {
		GL_CHECK(glGetShaderInfoLog(shader, logLength, nullptr, log.data()));
		return std::string(log.begin(), log.end());
	}

	return {};
}

/**
 * @brief Compiles a shader from source code and returns it as an RAII resource.
 *
 * The returned OpenGLResource owns the shader object name and will delete it when
 * destroyed (see createShader() in ogl_resource.hpp).
 *
 * @param aShaderType Shader stage enum (e.g. GL_VERTEX_SHADER).
 * @param aSource Full GLSL source code.
 * @return RAII-managed compiled shader.
 *
 * @throw ShaderCompilationError if compilation fails (includes compiler log).
 * @throw std::runtime_error if shader creation fails inside createShader().
 *
 * @note This function does not attach the shader to any program; use createShaderProgram().
 */
[[nodiscard]] inline auto compileShader(GLenum aShaderType, const std::string& aSource) {
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

/**
 * @brief Convenience alias for a set of compiled shader stage pointers.
 *
 * Each pointer is expected to point to a valid OpenGLResource that owns a compiled shader.
 *
 * @warning The pointed-to OpenGLResource objects must outlive createShaderProgram(...),
 *          because this function reads shader IDs from them during attach/link.
 */
using CompiledShaderStages = std::vector<const OpenGLResource *>;

/**
 * @brief Creates, links, and validates an OpenGL shader program from compiled shader stages.
 *
 * @param aShaderStages Pointers to compiled shader stage wrappers (e.g. vertex, fragment, compute).
 * @return RAII-managed linked shader program.
 *
 * @throw OpenGLError if linking or validation fails (includes program info log).
 * @throw std::runtime_error if program creation fails inside createShaderProgram().
 *
 * @note This attaches shaders, links, validates, and returns the program. It does not detach
 *       shaders after linking (which is optional in OpenGL).
 */
[[nodiscard]] inline auto createShaderProgram(const CompiledShaderStages &aShaderStages) {
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

/**
 * @brief Creates a shader program from vertex + fragment shader objects.
 *
 * @param vertexShader Compiled vertex shader (RAII wrapper).
 * @param fragmentShader Compiled fragment shader (RAII wrapper).
 * @return Linked and validated shader program (RAII wrapper).
 */
[[nodiscard]] inline auto createShaderProgram(const OpenGLResource& vertexShader, const OpenGLResource& fragmentShader) {
	return createShaderProgram(CompiledShaderStages{ &vertexShader, &fragmentShader });
}

/**
 * @brief Compiles vertex+fragment shaders from source strings and links them into a program.
 *
 * @param vertexShader Vertex shader source code.
 * @param fragmentShader Fragment shader source code.
 * @return Linked and validated shader program.
 *
 * @throw ShaderCompilationError if either stage fails to compile.
 * @throw OpenGLError if linking/validation fails.
 */
[[nodiscard]] inline auto createShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {
	auto vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	auto fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	return createShaderProgram(vs, fs);
}

#ifdef GL_COMPUTE_SHADER
/**
 * @brief Compiles a compute shader and links it into a program.
 *
 * @param computeShader Compute shader source code.
 * @return Linked and validated compute program.
 */
[[nodiscard]] inline auto createComputeShaderProgram(const std::string& computeShader) {
	auto cs = compileShader(GL_COMPUTE_SHADER, computeShader);

	return createShaderProgram(CompiledShaderStages{ &cs });
}
#endif

/**
 * @brief Lists active uniforms of a linked shader program.
 *
 * Uses glGetActiveUniform to enumerate active uniforms and glGetUniformLocation
 * to compute their locations.
 *
 * @param aShaderProgram Linked shader program (RAII wrapper).
 * @return Vector of UniformInfo entries (possibly empty).
 *
 * @note Uniform arrays are reported by the driver; names may include "[0]" depending on implementation.
 * @note Location may be -1 for uniforms that are inactive/optimized out or for certain built-ins.
 */
[[nodiscard]] inline std::vector<UniformInfo> listShaderUniforms(const OpenGLResource &aShaderProgram) {
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

/**
 * @brief Loads a shader source file into a string (binary-safe).
 *
 * Opens the file in binary mode to preserve exact contents (including line endings).
 *
 * @param filePath Path to a text file containing GLSL source code.
 * @return Entire file contents as a single string.
 *
 * @throw OpenGLError if the file does not exist.
 * @throw std::runtime_error if the file cannot be opened.
 */
[[nodiscard]] inline std::string loadShaderSource(const fs::path& filePath) {
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
