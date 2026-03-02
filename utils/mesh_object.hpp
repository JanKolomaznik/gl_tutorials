#pragma once

#include "scene_object.hpp"
#include "material_factory.hpp"
#include <filesystem>

namespace fs = std::filesystem;

class MeshObject: public SceneObject {
public:
	MeshObject() {};

	void addMaterial(std::string aMode, MaterialParameters aMaterialParams) {
		mRenderInfos[aMode].materialParams = aMaterialParams;
	}

	std::optional<RenderData> getRenderData(const RenderOptions &aOptions) const override {
		auto it = mRenderInfos.find(aOptions.mode);
		if (it == mRenderInfos.end()) {
			return std::optional<RenderData>();
		}

		if (!it->second.shaderProgram || !it->second.geometry) {
			return std::optional<RenderData>();
		}
		return std::optional<RenderData>(RenderData{
				getModelMatrix(),
				it->second.materialParams,
				*(it->second.shaderProgram),
				*(it->second.geometry)
			});
	}

	virtual std::shared_ptr<AGeometry> getGeometry(GeometryFactory &aGeometryFactory, RenderStyle aRenderStyle) = 0;

protected:
	void getTextures(MaterialParameterValues &aParams, MaterialFactory &aMaterialFactory) {
		for (auto &value : aParams) {
			TextureInfo * texture = std::get_if<TextureInfo>(&(value.second));
			if (!texture) {
				continue;
			}
			texture->textureData = aMaterialFactory.getTexture(texture->name);
		}
	}

	std::map<std::string, RenderInfo> mRenderInfos;
};

class LoadedMeshObject: public MeshObject {
public:
	LoadedMeshObject(const fs::path &aMeshPath)
       		: mMeshPath(aMeshPath)
	{
	}

	virtual std::shared_ptr<AGeometry> getGeometry(GeometryFactory &aGeometryFactory, RenderStyle aRenderStyle) {
 		return aGeometryFactory.loadMesh(mMeshPath, aRenderStyle);
	}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) override {
		for (auto &mode : mRenderInfos) {
			mode.second.shaderProgram = aMaterialFactory.getShaderProgram(mode.second.materialParams.mMaterialName);
			getTextures(mode.second.materialParams.mParameterValues, aMaterialFactory);
			mode.second.geometry = getGeometry(aGeometryFactory, mode.second.materialParams.mRenderStyle);
		}
	}
protected:
	fs::path mMeshPath;
};
