#pragma once

#include "mesh_object.hpp"


class Cube: public MeshObject {
public:
	Cube() {}

	virtual std::shared_ptr<AGeometry> getGeometry(GeometryFactory &aGeometryFactory, RenderStyle aRenderStyle) {
		switch (aRenderStyle) {
		case RenderStyle::Solid:
			return aGeometryFactory.getCubeNormTex();
		case RenderStyle::Wireframe:
			return aGeometryFactory.getCubeOutline();
		}
		return std::shared_ptr<AGeometry>();
	}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) override {
		for (auto &mode : mRenderInfos) {
			mode.second.shaderProgram = aMaterialFactory.getShaderProgram(mode.second.materialParams.mMaterialName);
			getTextures(mode.second.materialParams.mParameterValues, aMaterialFactory);
			mode.second.geometry = getGeometry(aGeometryFactory, mode.second.materialParams.mRenderStyle);
		}
	}
protected:
};

class Plane: public MeshObject {
public:
	Plane() {}

	virtual std::shared_ptr<AGeometry> getGeometry(GeometryFactory &aGeometryFactory, RenderStyle aRenderStyle) {
		switch (aRenderStyle) {
		case RenderStyle::Solid:
			return aGeometryFactory.getPlane();
		case RenderStyle::Wireframe:
			return aGeometryFactory.getPlaneOutline();
		}
		return std::shared_ptr<AGeometry>();
	}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) override {
		for (auto &mode : mRenderInfos) {
			mode.second.shaderProgram = aMaterialFactory.getShaderProgram(mode.second.materialParams.mMaterialName);
			getTextures(mode.second.materialParams.mParameterValues, aMaterialFactory);
			mode.second.geometry = getGeometry(aGeometryFactory, mode.second.materialParams.mRenderStyle);
		}
	}
protected:
};
