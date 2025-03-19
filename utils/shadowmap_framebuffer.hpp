#pragma once

#include <glad/glad.h>
#include <ogl_material_factory.hpp>

class ShadowmapFramebuffer {
public:
	ShadowmapFramebuffer(
		int aWidth,
		int aHeight)
		: mWidth(aWidth)
		, mHeight(aHeight)
		, mFramebuffer(createFramebuffer())
	{
		init();
	}

	void bind() {
		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffer.get()));
	}

	void unbind() {
		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	}

	void init() {
		bind();
		mDepthMap = std::make_shared<OGLTexture>(
				createDepthMap(mWidth, mHeight));
		checkStatus();
		unbind();
	}

	OpenGLResource createDepthMap(int aWidth, int aHeight)
	{
		auto depthMap = createTexture();
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, depthMap.get()));
		GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, aWidth, aHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));

		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

		// The border color is set to white, which is useful for shadow mapping to ensure
		// that areas outside the shadow map are always lit.
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

		// Attach the texture to the FBO
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap.get(), 0));

		return depthMap;
	}

	void setDrawBuffers() {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	void checkStatus() {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw OpenGLError("Shadowmap framebuffer is not complete!");
		}
	}

	std::shared_ptr<OGLTexture> getDepthMap() {
		return mDepthMap;
	}

	int mWidth;
	int mHeight;
	OpenGLResource mFramebuffer;
	std::shared_ptr<OGLTexture> mDepthMap;
};
