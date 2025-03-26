#pragma once

#include <vector>

#include "camera.hpp"
#include "ogl_material_factory.hpp"
#include "ogl_geometry_factory.hpp"


class Renderer {
public:

	Renderer(OGLMaterialFactory &aMaterialFactory)
		: mMaterialFactory(aMaterialFactory)
	{
		mShowNormalsShader = std::static_pointer_cast<OGLShaderProgram>(
				mMaterialFactory.getShaderProgram("generate_normals"));
	}



	void initialize() {
		GL_CHECK(glEnable(GL_DEPTH_TEST));
		GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
	}

	void clear() {
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	template<typename TScene, typename TCamera>
	void renderScene(const TScene &aScene, const TCamera &aCamera, RenderOptions aRenderOptions) {
		auto projection = aCamera.getProjectionMatrix();
		auto view = aCamera.getViewMatrix();

		std::vector<RenderData> renderData;
		for (const auto &object : aScene.getObjects()) {
			auto data = object.getRenderData(aRenderOptions);
			if (data) {
				renderData.push_back(data.value());
			}
		}

		MaterialParameterValues fallbackParameters;
		fallbackParameters["u_projMat"] = projection;
		fallbackParameters["u_viewMat"] = view;
		fallbackParameters["u_solidColor"] = glm::vec4(0,0,0,1);
		fallbackParameters["u_viewPos"] = aCamera.getPosition();
		fallbackParameters["u_near"] = aCamera.near();
		fallbackParameters["u_far"] = aCamera.far();

		GL_CHECK(glPatchParameteri(GL_PATCH_VERTICES, 3));
		for (const auto &data: renderData) {
			const glm::mat4 &modelMat = data.modelMat;
			const MaterialParameters &params = data.mMaterialParams;
			const OGLShaderProgram &shaderProgram = static_cast<const OGLShaderProgram &>(data.mShaderProgram);
			const OGLGeometry &geometry = static_cast<const OGLGeometry&>(data.mGeometry);

			fallbackParameters["u_modelMat"] = modelMat;
			fallbackParameters["u_normalMat"] = glm::mat3(modelMat);

			shaderProgram.use();
			shaderProgram.setMaterialParameters(params.mParameterValues, fallbackParameters);
			geometry.bind();
			if (params.mIsTesselation) {
				geometry.draw(GL_PATCHES);
			} else {
				geometry.draw();
			}
		}
	}

	template<typename TScene, typename TCamera>
	void renderSceneNormals(const TScene &aScene, const TCamera &aCamera, RenderOptions aRenderOptions) {
		auto projection = aCamera.getProjectionMatrix();
		auto view = aCamera.getViewMatrix();

		std::vector<RenderData> renderData;
		for (const auto &object : aScene.getObjects()) {
			auto data = object.getRenderData(aRenderOptions);
			if (data) {
				renderData.push_back(data.value());
			}
		}

		MaterialParameterValues fallbackParameters;
		fallbackParameters["u_projMat"] = projection;
		fallbackParameters["u_viewMat"] = view;
		fallbackParameters["u_solidColor"] = glm::vec4(0,0,0,1);
		fallbackParameters["u_viewPos"] = aCamera.getPosition();
		fallbackParameters["u_near"] = aCamera.near();
		fallbackParameters["u_far"] = aCamera.far();
		for (const auto &data: renderData) {
			const glm::mat4 &modelMat = data.modelMat;
			const OGLGeometry &geometry = static_cast<const OGLGeometry&>(data.mGeometry);

			fallbackParameters["u_modelMat"] = modelMat;
			fallbackParameters["u_normalMat"] = glm::mat3(modelMat);

			mShowNormalsShader->use();
			mShowNormalsShader->setMaterialParameters(fallbackParameters, {});

			geometry.bind();
			geometry.draw(GL_POINTS);
		}
	}
protected:
	std::shared_ptr<OGLShaderProgram> mShowNormalsShader;

	OGLMaterialFactory &mMaterialFactory;
};
