#pragma once

#include <stdexcept>
#include <string>

/**
 * @brief Exception class for OpenGL errors,
 *		extended to optionally store file and line information.
 */
class OpenGLError : public std::runtime_error {
public:
	/**
	 * @param message   Error description.
	 * @param errorCode OpenGL error code.
	 * @param file	  Source file (optional).
	 * @param line	  Line number (optional).
	 */
	OpenGLError(const std::string& message,
				GLenum errorCode = 0,
				const char* file = nullptr,
				int line = 0)
		: std::runtime_error(message),
		  errorCode(errorCode),
		  fileName(file ? file : ""), // Handle the nullptr gracefully
		  lineNumber(line) {}

	GLenum getErrorCode() const { return errorCode; }
	const std::string& getFileName() const { return fileName; }
	int getLineNumber() const { return lineNumber; }

private:
	GLenum	  errorCode;
	std::string fileName;   ///< Empty if file info wasn't provided
	int		 lineNumber; ///< 0 if line info wasn't provided
};

/**
 * @brief Retrieves a readable string for a given OpenGL error code.
 */
inline std::string getGLErrorString(GLenum errorCode)
{
	static const std::unordered_map<GLenum, std::string> errorMap = {
		{GL_NO_ERROR,                       "NO_ERROR"},
		{GL_INVALID_ENUM,                   "INVALID_ENUM"},
		{GL_INVALID_VALUE,                  "INVALID_VALUE"},
		{GL_INVALID_OPERATION,              "INVALID_OPERATION"},
		{GL_STACK_OVERFLOW,                 "STACK_OVERFLOW"},
		{GL_STACK_UNDERFLOW,                "STACK_UNDERFLOW"},
		{GL_OUT_OF_MEMORY,                  "OUT_OF_MEMORY"},
		{GL_INVALID_FRAMEBUFFER_OPERATION,  "INVALID_FRAMEBUFFER_OPERATION"},
	};

	auto it = errorMap.find(errorCode);
	return (it != errorMap.end()) ? it->second : "UNKNOWN_ERROR";
}

/**
 * @brief Checks for an OpenGL error. Throws OpenGLError if any is found.
 *
 * @param errorMessage Description of the context.
 * @param file  Source file for error context (default: nullptr).
 * @param line  Line number for error context (default: 0).
 */
inline void checkOpenGLError(
	const std::string& errorMessage,
	const char* file = nullptr,
	int line = 0)
{
	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		const std::string errorString = getGLErrorString(errorCode);

		// Build a full message that optionally includes file/line
		std::string fullMessage = errorMessage + " (" + errorString + ")";
		if (file) {
			fullMessage += " at " + std::string(file) + ":" + std::to_string(line);
		}

		throw OpenGLError(fullMessage, errorCode, file, line);
	}
}

// Enable/disable error checks based on build configuration
#if defined(DEBUG) || defined(_DEBUG) || defined(GL_DEBUG)
	#define GL_CHECK_ERROR_ENABLED 1
#else
	#define GL_CHECK_ERROR_ENABLED 0
#endif

/**
 * @brief Wraps an OpenGL call, then checks for errors using file and line info.
 */
#if GL_CHECK_ERROR_ENABLED
	#define GL_CHECK(x) do { \
		x; \
		checkOpenGLError(#x, __FILE__, __LINE__); \
	} while (0)
#else
	#define GL_CHECK(x) x
#endif
