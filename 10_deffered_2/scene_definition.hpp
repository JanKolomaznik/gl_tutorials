#pragma once

#include <memory>
#include <vector>
#include <ranges>
#include <random>

#include "scene_object.hpp"
#include "cube.hpp"

#include "material_factory.hpp"
#include "geometry_factory.hpp"
#include "simple_scene.hpp"


std::vector<glm::vec3> generatePalmPositions() {
	std::vector<glm::vec3> seeds = {
		glm::vec3(0.0f, 19.0f, 400.0f),
		glm::vec3(200.0f, 19.0f, 200.0f),
		glm::vec3(280.0f, 19.0f, 100.0f),
		glm::vec3(370.0f, 19.0f, -50.0f),
		glm::vec3(-250.0f, 19.0f, 230.0f),
	};

	// return seeds;
	float maxRadius = 100;
	int count = 130;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(-maxRadius, maxRadius);

	std::vector<glm::vec3> points;
	for (auto &seed : seeds) {
		int generated = 0;
		while (generated < count) {
			float x = dist(gen);
			float y = dist(gen);
			if ((x*x + y*y) < (maxRadius*maxRadius)) {
				glm::vec3 point = glm::vec3(seed.x + x, seed.y, seed.z + y);
				points.push_back(point);
				++generated;
			}
		}
	}
	return points;
}


inline SimpleScene createIslandScene(MaterialFactory& aMaterialFactory, GeometryFactory& aGeometryFactory) {
	SimpleScene scene;
	auto palleteMaterial = MaterialParameters(
			"material_deffered",
			RenderStyle::Solid,
			{
				{ "u_diffuseTexture", TextureInfo("island/pallete.png") },
			}
			);
	{
		auto island = std::make_shared<LoadedMeshObject>("./data/geometry/island/island.obj");
		island->addMaterial("solid", palleteMaterial);
		island->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(island);
	}
	{
		auto sea = std::make_shared<LoadedMeshObject>("./data/geometry/island/sea.obj");
		sea->addMaterial(
			"solid",
			MaterialParameters(
				"material_deffered",
				RenderStyle::Solid,
				{
					{ "u_diffuseTexture", TextureInfo("island/sea.png") },
				}
				)
		);
		sea->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(sea);
	}
	{
		auto ship = std::make_shared<LoadedMeshObject>("./data/geometry/island/ship.obj");
		ship->setPosition(glm::vec3(430.0f, -6.0f, 450.0f));
		ship->addMaterial("solid", palleteMaterial);
		ship->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(ship);
	}
	{
		auto ship = std::make_shared<LoadedMeshObject>("./data/geometry/island/ship.obj");
		ship->setPosition(glm::vec3(350.0f, -6.0f, 500.0f));
		ship->setRotation(glm::vec3(0.0f, glm::radians(70.0f), 0.0f));
		ship->addMaterial("solid", palleteMaterial);
		ship->prepareRenderData(aMaterialFactory, aGeometryFactory);
		scene.addObject(ship);
	}
	{
		for (auto position : generatePalmPositions()) {
			auto palm = std::make_shared<LoadedMeshObject>("./data/geometry/island/palm_01.obj");
			palm->setPosition(position);
			palm->setScale(glm::vec3(5.0f));
			palm->addMaterial("solid", palleteMaterial);
			palm->prepareRenderData(aMaterialFactory, aGeometryFactory);
			scene.addObject(palm);
		}
	}

	return scene;
}
