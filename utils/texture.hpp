#pragma once

#include <glad/glad.h>
#include "ogl_resource.hpp"

inline OpenGLResource createColorTexture(
		int aWidth,
		int aHeight,
		GLint aInternalFormat = GL_RGBA,
		GLenum aFormat = GL_RGBA,
		GLenum aType = GL_FLOAT)
{
	auto textureID = createTexture();
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID.get()));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, aInternalFormat, aWidth, aHeight, 0, aFormat, aType, NULL));

	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	return textureID;
}
