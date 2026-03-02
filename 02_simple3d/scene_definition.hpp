#pragma once

#include <memory>
#include <vector>
#include <ranges>

#include "scene_object.hpp"
#include "cube.hpp"

#include "material_factory.hpp"
#include "geometry_factory.hpp"
#include "simple_scene.hpp"


inline SimpleScene createCubeScene(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) {
	SimpleScene scene;
	auto cube = std::make_shared<Cube>();

	cube->setName("CUBE");
	cube->addMaterial(
		"solid",
		MaterialParameters(
			"solid_color",
			RenderStyle::Solid,
			{
				{ "u_solidColor", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)}
			})
		);
	cube->prepareRenderData(aMaterialFactory, aGeometryFactory);

	// cube->setRotation(glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f)));
	scene.addObject(cube);
	return scene;
}

