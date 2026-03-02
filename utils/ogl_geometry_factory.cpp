#include "ogl_geometry_factory.hpp"

#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <array>
#include "vertex.hpp"

#include "obj_file_loading.hpp"
#include "ogl_geometry_construction.hpp"

std::shared_ptr<AGeometry> OGLGeometryFactory::getAxisGizmo() {
	auto it = mObjects.find("axisGizmo");
	if (it != mObjects.end()) {
		return it->second;
	}

	auto geometry = std::make_shared<OGLGeometry>(generateAxisGizmo());
	mObjects["axisGizmo"] = geometry;
	return geometry;
}

std::shared_ptr<AGeometry> OGLGeometryFactory::getCube() {
	auto it = mObjects.find("cube");
	if (it != mObjects.end()) {
		return it->second;
	}

	auto geometry = std::make_shared<OGLGeometry>(generateCubeBuffers());
	mObjects["cube"] = geometry;
	return geometry;
}

std::shared_ptr<AGeometry> OGLGeometryFactory::getCubeOutline() {
	auto it = mObjects.find("cubeOutline");
	if (it != mObjects.end()) {
		return it->second;
	}

	auto geometry = std::make_shared<OGLGeometry>(generateCubeOutlineBuffers());
	mObjects["cubeOutline"] = geometry;
	return geometry;
}

std::shared_ptr<AGeometry> OGLGeometryFactory::getCubeNormTex() {
	auto it = mObjects.find("cubeNormTex");
	if (it != mObjects.end()) {
		return it->second;
	}

	auto geometry = std::make_shared<OGLGeometry>(generateCubeBuffersNormTex());
	mObjects["cubeNormTex"] = geometry;
	return geometry;
}

std::shared_ptr<AGeometry> OGLGeometryFactory::getPlane() {
	auto it = mObjects.find("plane");
	if (it != mObjects.end()) {
		return it->second;
	}

	auto geometry = std::make_shared<OGLGeometry>(generatePlaneBuffers());
	mObjects["plane"] = geometry;
	return geometry;
}

std::shared_ptr<AGeometry> OGLGeometryFactory::getPlaneOutline() {
	auto it = mObjects.find("planeOutline");
	if (it != mObjects.end()) {
		return it->second;
	}

	auto geometry = std::make_shared<OGLGeometry>(generatePlaneOutlineBuffers());
	mObjects["getPlaneOutline"] = geometry;
	return geometry;
}

std::shared_ptr<AGeometry> OGLGeometryFactory::loadMesh(fs::path aMeshPath, RenderStyle aRenderStyle) {
	aMeshPath = fs::canonical(aMeshPath);
	auto it = mObjects.find(aMeshPath.string());
	if (it != mObjects.end()) {
		return it->second;
	}
	auto mesh = loadOBJ(aMeshPath);

	auto geometry = std::make_shared<OGLGeometry>(generateMeshBuffersNormTex(mesh));

	mObjects[aMeshPath.string()] = geometry;
	return geometry;
}

