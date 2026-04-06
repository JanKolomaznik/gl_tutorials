#pragma once

#include <memory>
#include <vector>
#include <ranges>

#include "vertex.hpp"
#include "scene_object.hpp"
#include "cube.hpp"

#include "material_factory.hpp"
#include "geometry_factory.hpp"
#include "simple_scene.hpp"

constexpr unsigned int DIFFUSE = 1;
constexpr unsigned int SPECULAR = 1 << 1;
constexpr unsigned int BUMP = 1 << 2;
constexpr unsigned int PARALLAX = 1 << 3;
constexpr unsigned int AMBIENT_OCC = 1 << 4;
constexpr unsigned int SHADOW = 1 << 5;
constexpr unsigned int DEBUG = 1 << 7;


inline SimpleScene createSphereScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	{
		auto cube = std::make_shared<Cube>();

		cube->setName("CUBE1");
		// cube->setPosition(glm::vec3(0.75f, 0.0f, 0.0f));
		cube->setScale(glm::vec3(2.0, 1.0f, 1.0f));
		cube->addMaterial(
			"raycast",
			MaterialParameters(
				"sphere_raycast",
				RenderStyle::Solid,
				{
				}
				)
			);
		cube->addMaterial(
			"wireframe",
			MaterialParameters(
				"solid_color",
				RenderStyle::Wireframe,
				{}
				)
			);
		cube->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(cube);
	}
	return scene;
}

inline SimpleScene createAnimationScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	{
		auto plane = std::make_shared<Plane>();

		plane->setName("Plane");
		// cube->setPosition(glm::vec3(0.75f, 0.0f, 0.0f));
		// cube->setScale(glm::vec3(2.0, 1.0f, 1.0f));
		plane->addMaterial(
			"solid",
			MaterialParameters(
				"animated_texture",
				RenderStyle::Solid,
				{
					{ "u_volumeData", TextureInfo("intestine.dump") },
					{ "u_intensityMultiplier", 30.0f },
					{ "u_speed", 0.5f },
				}
				)
			);
		plane->addMaterial(
			"wireframe",
			MaterialParameters(
				"solid_color",
				RenderStyle::Wireframe,
				{}
				)
			);
		plane->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(plane);
	}
	return scene;
}

inline SimpleScene createMIPScene1(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	{
		auto cube = std::make_shared<Cube>();

		cube->setName("CUBE1");
		cube->setScale(glm::vec3(1.0, 1.0f, 1.5f));
		cube->addMaterial(
			"raycast",
			MaterialParameters(
				"mip",
				RenderStyle::Solid,
				{
					{ "u_volumeData", TextureInfo("intestine.dump") },
					{ "u_intensityMultiplier", 30.0f },
					{ "stepSize", 0.01f },
				}
				)
			);
		cube->addMaterial(
			"wireframe",
			MaterialParameters(
				"solid_color",
				RenderStyle::Wireframe,
				{
				}
				)
			);
		cube->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(cube);
	}
	return scene;
}

inline SimpleScene createMIPScene2(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	{
		auto cube = std::make_shared<Cube>();

		cube->setName("CUBE1");
		cube->setScale(glm::vec3(1.0, 1.0f, 1.0f));
		cube->addMaterial(
			"raycast",
			MaterialParameters(
				"mip",
				RenderStyle::Solid,
				{
					{ "u_volumeData", TextureInfo("lebka1.dump") },
					// { "u_volumeData", TextureInfo("vertebra16.mhd") },
					// { "u_volumeData", TextureInfo("mrt16_angio2.mhd") },
					{ "u_intensityMultiplier", 20.0f },
					{ "stepSize", 0.01f },
				}
				)
			);
		cube->addMaterial(
			"wireframe",
			MaterialParameters(
				"solid_color",
				RenderStyle::Wireframe,
				{
				}
				)
			);
		cube->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(cube);
	}
	return scene;
}

