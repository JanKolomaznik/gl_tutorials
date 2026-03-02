#pragma once

#include "material_factory.hpp"

class AGeometry {
public:
	AGeometry() {}
	virtual ~AGeometry() {}
};

class GeometryFactory {
public:

	virtual std::shared_ptr<AGeometry> getAxisGizmo() = 0;
	virtual std::shared_ptr<AGeometry> getCube() = 0;
	virtual std::shared_ptr<AGeometry> getCubeOutline() = 0;
	virtual std::shared_ptr<AGeometry> getCubeNormTex() = 0;

	virtual std::shared_ptr<AGeometry> getPlane() = 0;
	virtual std::shared_ptr<AGeometry> getPlaneOutline() = 0;

	virtual std::shared_ptr<AGeometry> loadMesh(fs::path aMeshPath, RenderStyle aRenderStyle) = 0;
};

