#pragma once

#include <memory>
#include <vector>
#include <ranges>

#include "scene_object.hpp"
#include "cube.hpp"

#include "material_factory.hpp"
#include "geometry_factory.hpp"
#include "simple_scene.hpp"


inline SimpleScene createGizmoScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto gizmo = std::make_shared<AxisGizmo>();
	gizmo->prepareRenderData(aMaterialFactory, aGeometryFactory);
	scene.addObject(gizmo);
	return scene;
}

inline SimpleScene createTexturedCubeScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto cube = std::make_shared<Cube>();

	cube->setName("CUBE");
	cube->addMaterial(
		"solid",
		MaterialParameters(
			"texture_blend",
			RenderStyle::Solid,
			{
				{ "u_textureSampler1", TextureInfo("brick_wall/Brick_Wall_012_COLOR.jpg") },
				{ "u_textureSampler2", TextureInfo("pebbles/Pebbles_003_COLOR.jpg") }
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
	return scene;
}


inline SimpleScene createMonkeyScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto mesh = std::make_shared<LoadedMeshObject>("./data/geometry/monkey.obj");

	mesh->setScale(glm::vec3(0.5));
	mesh->setName("MONKEY");
	// mesh->addMaterial(
	// 	"solid",
	// 	MaterialParameters(
	// 		"solid_color",
	// 		RenderStyle::Solid,
	// 		{
	// 			{"u_solidColor", glm::vec4(0,0.5,0.5,1)}
	// 		})
	// 	);
	mesh->addMaterial(
		"solid",
		MaterialParameters(
			"phong",
			RenderStyle::Solid,
			{
				{ "light.ambient", glm::vec3(0.3, 0.3, 0.3) },
				{ "light.diffuse", glm::vec3(0.6, 0.6, 0.6) },
				{ "light.position", glm::vec3(3.0, 5.0, 6.0) },
				{ "light.specular", glm::vec3(1.0, 1.0, 1.0) },
				{ "material.ambient", glm::vec3(0.1, 0.1, 0.1) },
				{ "material.diffuse", glm::vec3(1.0, 0.5, 0.31) },
				{ "material.shininess", 32.0f },
				{ "material.specular", glm::vec3(0.5, 0.5, 0.5) },
			}
			)
		);
	mesh->addMaterial(
		"wireframe",
		MaterialParameters(
			"solid_color",
			RenderStyle::Wireframe,
			{}
			)
		);
	mesh->prepareRenderData(aMaterialFactory, aGeometryFactory);

	scene.addObject(mesh);
	return scene;
}
