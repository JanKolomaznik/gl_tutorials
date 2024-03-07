#pragma once

#include <stdexcept>
#include <string>

class OpenGLError : public std::runtime_error {
public:
	OpenGLError(const std::string& message, GLenum errorCode=0)
		: std::runtime_error(message), errorCode(errorCode) {}

	GLenum getErrorCode() const { return errorCode; }

private:
	GLenum errorCode;
};

inline void checkOpenGLError(const std::string& errorMessage) {
	GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		std::string errorString;
		switch (errorCode) {
			case GL_INVALID_ENUM:                  errorString = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 errorString = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             errorString = "INVALID_OPERATION"; break;
#ifndef  __APPLE__
			case GL_STACK_OVERFLOW:                errorString = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               errorString = "STACK_UNDERFLOW"; break;
#endif
			case GL_OUT_OF_MEMORY:                 errorString = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: errorString = "INVALID_FRAMEBUFFER_OPERATION"; break;
			default:                               errorString = "UNKNOWN_ERROR"; break;
		}
		throw OpenGLError(errorMessage + ": " + errorString, errorCode);
	}
}


// Define GL_DEBUG to enable error checks in non-debug builds if needed
#if defined(DEBUG) || defined(_DEBUG) || defined(GL_DEBUG)
	#define GL_CHECK_ERROR_ENABLED 1
#else
	#define GL_CHECK_ERROR_ENABLED 0
#endif

#if GL_CHECK_ERROR_ENABLED
#define GL_CHECK(x) do { \
		x; \
		checkOpenGLError(#x); \
	} while (0)
#else
	#define GL_CHECK(x) x
#endif
