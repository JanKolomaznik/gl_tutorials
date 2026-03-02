#pragma once

#include <memory>
#include <vector>
#include <ranges>

#include "scene_object.hpp"

class SimpleScene {
public:
	void addObject(std::shared_ptr<SceneObject> aNewObject) {
		mObjects.push_back(aNewObject);
	}

	auto getObjects() const {
		return mObjects | std::views::transform([](const auto &aPtr) -> const SceneObject& { return *aPtr; });
	}
protected:
	std::vector<std::shared_ptr<SceneObject>> mObjects;
};


