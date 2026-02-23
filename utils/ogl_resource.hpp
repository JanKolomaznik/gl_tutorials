#pragma once

#include <glad/glad.h>
#include <functional>
#include <stdexcept>
#include <utility>

#include "error_handling.hpp"

/**
 * @class OpenGLResource
 * @brief Move-only RAII wrapper for a single OpenGL object name (GLuint).
 *
 * This class owns exactly one OpenGL object handle (ID) and destroys it in the
 * destructor using a user-provided deleter function.
 *
 * Typical usage is to construct it via one of the helper factory functions
 * (e.g. createBuffer(), createVertexArray()) which bind the correct glGen/glDelete
 * pair.
 *
 * @note This wrapper does not manage OpenGL context lifetime. The context must
 *       remain valid while the resource is destroyed (i.e. when the destructor runs).
 * @note The wrapper is move-only to model unique ownership of the OpenGL handle.
 *
 * @warning Destruction and reassignment require a valid OpenGL context (driver-dependent).
 */
class OpenGLResource {
public:
	/**
	 * @brief Constructs an empty wrapper (no resource owned).
	 *
	 * The resulting object evaluates to false in boolean context.
	 * @see operator bool()
	 */
	OpenGLResource()
		: mId(0)
	{}

	/**
	 * @brief Constructs and owns a new OpenGL resource using the provided callbacks.
	 *
	 * The constructor stores the callbacks and immediately calls @p aCreateFunc
	 * to obtain an OpenGL object name (ID).
	 *
	 * @param aCreateFunc Function returning a newly created OpenGL resource ID.
	 *        The returned ID is stored as the owned handle.
	 * @param aDeleteFunc Function used to delete the owned resource ID.
	 *
	 * @note The callbacks are type-erased via std::function to allow lambdas,
	 *       function pointers, and functors.
	 * @warning If @p aCreateFunc returns 0, the wrapper will be considered empty/invalid
	 *          (operator bool() == false) and no deletion will occur.
	 */
	OpenGLResource(
		std::function<GLuint()> aCreateFunc,
		std::function<void(GLuint)> aDeleteFunc)
		: mCreateFunc(aCreateFunc)
		, mDeleteFunc(aDeleteFunc)
	{
		mId = mCreateFunc();
	}

	/**
	 * @brief Destroys the owned OpenGL resource (if a deleter is set and ID is non-zero).
	 *
	 * @note OpenGL's object name 0 is reserved as "null". This wrapper treats 0 as "no resource".
	 * @warning Ensure the OpenGL context that owns this resource is still current/valid
	 *          at destruction time, otherwise behavior is undefined (driver-dependent).
	 */
	~OpenGLResource() {
		if (mDeleteFunc && (mId != 0)) {
			mDeleteFunc(mId);
		}
	}

	/// @name Non-copyable
	/// @{
	/**
	 * @brief Copy construction is disabled (unique ownership).
	 */
	OpenGLResource(const OpenGLResource&) = delete;

	/**
	 * @brief Copy assignment is disabled (unique ownership).
	 */
	OpenGLResource& operator=(const OpenGLResource&) = delete;
	/// @}

	/// @name Movable
	/// @{
	/**
	 * @brief Move constructor; transfers ownership from @p other.
	 *
	 * After the move, @p other no longer owns a resource (its ID is set to 0).
	 *
	 * @param other Resource wrapper to move from.
	 */
	OpenGLResource(OpenGLResource&& other) noexcept
		: mId(std::exchange(other.mId, 0))
		, mCreateFunc(std::move(other.mCreateFunc))
		, mDeleteFunc(std::move(other.mDeleteFunc))
	{}

	/**
	 * @brief Move assignment; releases current resource (if any) and takes ownership from @p other.
	 *
	 * If this wrapper currently owns a resource and has a deleter set, the deleter
	 * is invoked before taking ownership of @p other.
	 *
	 * After the assignment, @p other no longer owns a resource (its ID is set to 0).
	 *
	 * @param other Resource wrapper to move from.
	 * @return Reference to *this.
	 */
	OpenGLResource& operator=(OpenGLResource&& other) noexcept {
		if (this != &other) {
			if (mDeleteFunc && (mId != 0)) {
				mDeleteFunc(mId);
			}
			mId = std::exchange(other.mId, 0);
			mCreateFunc = std::move(other.mCreateFunc);
			mDeleteFunc = std::move(other.mDeleteFunc);
		}
		return *this;
	}
	/// @}

	/**
	 * @brief Returns the owned OpenGL object name (ID).
	 *
	 * @return The underlying OpenGL resource ID, or 0 if no resource is owned.
	 */
	[[nodiscard]] GLuint get() const { return mId; }

	/**
	 * @brief Releases ownership of the OpenGL ID without deleting it.
	 *
	 * After calling release(), this wrapper becomes empty (get() == 0) and will
	 * not delete the returned ID.
	 *
	 * @return The previously owned OpenGL resource ID (may be 0).
	 */
	[[nodiscard]] GLuint release() {
		return std::exchange(mId, 0);
	}

	/**
	 * @brief Deletes the currently owned resource (if any) and adopts @p aNewId.
	 *
	 * This is useful to explicitly manage lifetime, reinitialize resources, or
	 * adopt externally created OpenGL IDs.
	 *
	 * @param aNewId The new OpenGL object name to adopt (0 means "become empty").
	 *
	 * @warning This does not change the stored deleter. You should only reset() to IDs
	 *          that match the deleter semantics of this wrapper instance.
	 */
	void reset(GLuint aNewId = 0) {
		if (mDeleteFunc && (mId != 0)) {
			mDeleteFunc(mId);
		}
		mId = aNewId;
	}

	/**
	 * @brief Checks whether this wrapper currently owns a non-zero OpenGL ID.
	 *
	 * This enables convenient usage in conditionals.
	 *
	 * Example:
	 * \code
	 * OpenGLResource buf = createBuffer();
	 * if (buf) {
	 *     // Safe to use buf.get() as an OpenGL object name.
	 * }
	 * \endcode
	 *
	 * @return True if the owned ID is non-zero, otherwise false.
	 */
	explicit operator bool() const {
		return mId != 0;
	}

private:
	/**
	 * @brief The owned OpenGL object name.
	 *
	 * A value of 0 denotes "no resource owned" (the OpenGL reserved null name).
	 */
	GLuint mId = 0; ///< The OpenGL resource ID.

	/**
	 * @brief Creation callback used by the non-default constructor.
	 *
	 * Stored for completeness and move transfer; not used after construction
	 * in the current implementation.
	 */
	std::function<GLuint(void)> mCreateFunc;

	/**
	 * @brief Deletion callback invoked by the destructor and by reset()/move assignment cleanup.
	 */
	std::function<void(GLuint)> mDeleteFunc;
};

/**
 * @brief Creates an RAII-managed Vertex Array Object (VAO).
 *
 * Wraps glGenVertexArrays/glDeleteVertexArrays.
 *
 * @return OpenGLResource owning a VAO object name.
 * @note Requires a valid OpenGL context. Error checking is performed via GL_CHECK
 *       in the generator path.
 */
[[nodiscard]] inline OpenGLResource createVertexArray() {
	return OpenGLResource(
		[]{
			GLuint id = 0;
			GL_CHECK(glGenVertexArrays(1, &id));
			return id;
		},
		[](GLuint id){
			glDeleteVertexArrays(1, &id);
		});
}

/**
 * @brief Creates an RAII-managed Buffer Object.
 *
 * Wraps glGenBuffers/glDeleteBuffers.
 *
 * @return OpenGLResource owning a buffer object name.
 */
[[nodiscard]] inline OpenGLResource createBuffer() {
	return OpenGLResource(
		[]{
			GLuint id = 0;
			GL_CHECK(glGenBuffers(1, &id));
			return id;
		},
		[](GLuint id){
			glDeleteBuffers(1, &id);
		});
}

/**
 * @brief Creates an RAII-managed Query Object.
 *
 * Wraps glGenQueries/glDeleteQueries.
 *
 * @return OpenGLResource owning a query object name.
 */
[[nodiscard]] inline OpenGLResource createQuery() {
	return OpenGLResource(
		[]{
			GLuint id = 0;
			GL_CHECK(glGenQueries(1, &id));
			return id;
		},
		[](GLuint id){
			glDeleteQueries(1, &id);
		});
}

/**
 * @brief Creates an RAII-managed Renderbuffer Object.
 *
 * Wraps glGenRenderbuffers/glDeleteRenderbuffers.
 *
 * @return OpenGLResource owning a renderbuffer object name.
 */
[[nodiscard]] inline OpenGLResource createRenderBuffer() {
	return OpenGLResource(
		[]{
			GLuint id = 0;
			GL_CHECK(glGenRenderbuffers(1, &id));
			return id;
		},
		[](GLuint id){
			glDeleteRenderbuffers(1, &id);
		});
}

/**
 * @brief Creates an RAII-managed Framebuffer Object.
 *
 * Wraps glGenFramebuffers/glDeleteFramebuffers.
 *
 * @return OpenGLResource owning a framebuffer object name.
 */
[[nodiscard]] inline OpenGLResource createFramebuffer() {
	return OpenGLResource(
		[]{
			GLuint id = 0;
			GL_CHECK(glGenFramebuffers(1, &id));
			return id;
		},
		[](GLuint id){
			glDeleteFramebuffers(1, &id);
		});
}

/**
 * @brief Creates an RAII-managed Shader Object.
 *
 * Wraps glCreateShader/glDeleteShader.
 *
 * @param aShaderType OpenGL shader stage (e.g. GL_VERTEX_SHADER, GL_FRAGMENT_SHADER).
 * @return OpenGLResource owning a shader object name.
 *
 * @throw std::runtime_error if glCreateShader returns 0 (creation failed).
 */
[[nodiscard]] inline OpenGLResource createShader(GLenum aShaderType) {
	return OpenGLResource(
		[aShaderType]{
			const GLuint id = glCreateShader(aShaderType);
			if (id == 0) {
				throw std::runtime_error("glCreateShader failed (returned 0).");
			}
			return id;
		},
		[](GLuint id){
			glDeleteShader(id);
		});
}

/**
 * @brief Creates an RAII-managed Shader Program Object.
 *
 * Wraps glCreateProgram/glDeleteProgram.
 *
 * @return OpenGLResource owning a program object name.
 *
 * @throw std::runtime_error if glCreateProgram returns 0 (creation failed).
 */
[[nodiscard]] inline OpenGLResource createShaderProgram() {
	return OpenGLResource(
		[]{
			const GLuint id = glCreateProgram();
			if (id == 0) {
				throw std::runtime_error("glCreateProgram failed (returned 0).");
			}
			return id;
		},
		[](GLuint id){
			glDeleteProgram(id);
		});
}

/**
 * @brief Creates an RAII-managed Texture Object.
 *
 * Wraps glGenTextures/glDeleteTextures.
 *
 * @return OpenGLResource owning a texture object name.
 */
[[nodiscard]] inline OpenGLResource createTexture() {
	return OpenGLResource(
		[]{
			GLuint id = 0;
			GL_CHECK(glGenTextures(1, &id));
			return id;
		},
		[](GLuint id){
			glDeleteTextures(1, &id);
		});
}

/**
 * @brief Creates an RAII-managed Sampler Object.
 *
 * Wraps glGenSamplers/glDeleteSamplers.
 *
 * @return OpenGLResource owning a sampler object name.
 */
[[nodiscard]] inline OpenGLResource createSampler() {
	return OpenGLResource(
		[]{
			GLuint id = 0;
			GL_CHECK(glGenSamplers(1, &id));
			return id;
		},
		[](GLuint id){
			glDeleteSamplers(1, &id);
		});
}
