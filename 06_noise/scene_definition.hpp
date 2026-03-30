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


inline SimpleScene createNoiseDemonstrationScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	{
		auto plane = std::make_shared<Plane>();

		plane->setName("Plane");
		plane->setPosition(glm::vec3(-0.6f, 0.6f, 0.0f));
		plane->addMaterial(
			"solid",
			MaterialParameters(
				"noise_2d",
				RenderStyle::Solid,
				{
					{"u_noiseScale", 15.0f},
					{"u_selectedNoise", 0}
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
	{
		auto plane = std::make_shared<Plane>();

		plane->setName("Plane");
		plane->setPosition(glm::vec3(0.6f, 0.6f, 0.0f));
		plane->addMaterial(
			"solid",
			MaterialParameters(
				"noise_2d",
				RenderStyle::Solid,
				{
					{"u_noiseScale", 10.0f},
					{"u_selectedNoise", 1}
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
	{
		auto plane = std::make_shared<Plane>();

		plane->setName("Plane");
		plane->setPosition(glm::vec3(-0.6f, -0.6f, 0.0f));
		plane->addMaterial(
			"solid",
			MaterialParameters(
				"noise_animated",
				RenderStyle::Solid,
				{
					{"u_noiseScale", 15.0f},
					{"u_selectedNoise", 0}
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
	{
		auto plane = std::make_shared<Plane>();

		plane->setName("Plane");
		plane->setPosition(glm::vec3(0.6f, -0.6f, 0.0f));
		plane->addMaterial(
			"solid",
			MaterialParameters(
				"noise_animated",
				RenderStyle::Solid,
				{
					{"u_noiseScale", 10.0f},
					{"u_selectedNoise", 1}
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

inline SimpleScene createMonkeyScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	{
		auto mesh = std::make_shared<LoadedMeshObject>("./data/geometry/monkey.obj");
		mesh->setScale(glm::vec3(0.5));
		// mesh->setPosition(glm::vec3(-0.7, 0.0f, 0.0f));
		mesh->setRotation(glm::vec3(0.0f, glm::radians(180.0f), 0.0f));
		mesh->setName("MONKEY1");
		mesh->addMaterial(
				"solid",
				MaterialParameters(
					"noise_3d",
					RenderStyle::Solid,
					{
						{"u_noiseScale", 10.0f},
						{"u_selectedNoise", 1}
					})
				);
		mesh->addMaterial(
				"wireframe",
				MaterialParameters(
					"solid_color",
					RenderStyle::Wireframe,
					{
					})
				);
		mesh->prepareRenderData(aMaterialFactory, aGeometryFactory);

		scene.addObject(mesh);
	}
	return scene;
}


inline SimpleScene createTerrainScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	{
		auto plane = std::make_shared<Plane>();
		// auto plane = std::make_shared<LoadedMeshObject>("./data/geometry/plane.obj");

		plane->setName("Plane");
		// plane->setRotation(glm::vec3(glm::radians(70.0f), 0.0f, 0.0f));
		plane->setScale(glm::vec3(2.0f));
		plane->addMaterial(
			"solid",
			MaterialParameters(
				"terrain",
				RenderStyle::Solid,
				{
					{"u_inner", 32},
					{"u_outer", 32},
					{"u_selectedNoise", 1},
					{"u_maxDisplacement", 0.3f},
					{"u_noiseScale", 2.0f},
				},
				true)
			);
		plane->addMaterial(
			"wireframe",
			MaterialParameters(
				"terrain_solid_color",
				RenderStyle::Wireframe,
				{
					{"u_inner", 32},
					{"u_outer", 32},
					{"u_selectedNoise", 1},
					{"u_maxDisplacement", 0.3f},
					{"u_noiseScale", 2.0f},
					{"u_solidColor", glm::vec4(0,0,0,1)}

				},
				true)
			);
		plane->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(plane);
	}
	return scene;
}
