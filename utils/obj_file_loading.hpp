#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>

#include "vertex.hpp"

namespace fs = std::filesystem;


struct ObjMesh {
	std::vector<VertexNormTex> vertices;
	std::vector<unsigned int> indices;
};

ObjMesh loadOBJ(const fs::path& aObjPath);

