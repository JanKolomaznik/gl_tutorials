#pragma once

#include <vector>
#include "vertex.hpp"
#include "mesh_object.hpp"
#include "ogl_geometry_construction.hpp"
#include "ogl_geometry_factory.hpp"

inline
IndexedBuffer
generateInstancedCubeBuffers(const std::vector<VertexColor> &aPositionColorAttribs) {
	IndexedBuffer buffers {
		createVertexArray(),
	};
	buffers.vbos.push_back(createBuffer());
	buffers.vbos.push_back(createBuffer());
	buffers.vbos.push_back(createBuffer());

	std::vector<VertexNormTex> vertices;
	std::vector<unsigned int> indices;
	for (int i = 0; i < 3; ++i) {
		for (int direction = -1; direction < 2; direction +=2) {
			unsigned indexOffset = unsigned(vertices.size());
			for (int j = 0; j < 4; ++j) {
				vertices.push_back(VertexNormTex(
					insertDimension(unitFaceVertices[j], i, direction * 0.5f),
					insertDimension(glm::vec2(), i, float(direction)),
					unitFaceVertices[j] + glm::vec2(0.5f, 0.5f)));
			}

			for (auto index : faceTriangleIndices) {
				indices.push_back(index + indexOffset);
			}
		}
	}

	GL_CHECK(glBindVertexArray(buffers.vao.get()));

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffers.vbos[0].get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormTex) * vertices.size(), vertices.data(), GL_STATIC_DRAW));

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.vbos[1].get()));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW));

	// Position attribute
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex), (void*)0));
	GL_CHECK(glEnableVertexAttribArray(0));

	// Normal attribute
	GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex), (void*)(sizeof(glm::vec3))));
	GL_CHECK(glEnableVertexAttribArray(1));

	// Texture coordinate attribute
	GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormTex), (void*)(2*sizeof(glm::vec3))));
	GL_CHECK(glEnableVertexAttribArray(2));

	// INSTANCE ATTRIBUTES

	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, buffers.vbos[2].get()));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexColor) * aPositionColorAttribs.size(), aPositionColorAttribs.data(), GL_STATIC_DRAW));

	GL_CHECK(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor), (void*)(0)));
	GL_CHECK(glEnableVertexAttribArray(3));
	GL_CHECK(glVertexAttribDivisor(3, 1)); // 1 means the attribute advances once per instance

	GL_CHECK(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor), (void*)(sizeof(glm::vec3))));
	GL_CHECK(glEnableVertexAttribArray(4));
	GL_CHECK(glVertexAttribDivisor(4, 1)); // 1 means the attribute advances once per instance

	// Unbind VAO
	GL_CHECK(glBindVertexArray(0));

	buffers.indexCount = unsigned(indices.size());
	buffers.instanceCount = unsigned(aPositionColorAttribs.size());
	buffers.mode = GL_TRIANGLES;
	return buffers;
}


class InstancedCube: public MeshObject {
public:
	InstancedCube(std::vector<VertexColor> aInstanceAttributes)
		: mInstanceAttributes(std::move(aInstanceAttributes))

	{}

	virtual std::shared_ptr<AGeometry> getGeometry(GeometryFactory &aGeometryFactory, RenderStyle aRenderStyle) {
		return std::make_shared<OGLGeometry>(generateInstancedCubeBuffers(mInstanceAttributes));
		// switch (aRenderStyle) {
		// case RenderStyle::Solid:
		// 	return aGeometryFactory.getCubeNormTex();
		// case RenderStyle::Wireframe:
		// 	return aGeometryFactory.getCubeOutline();
		// }
		// return std::shared_ptr<AGeometry>();
	}

	void prepareRenderData(MaterialFactory &aMaterialFactory, GeometryFactory &aGeometryFactory) override {
		for (auto &mode : mRenderInfos) {
			mode.second.shaderProgram = aMaterialFactory.getShaderProgram(mode.second.materialParams.mMaterialName);
			getTextures(mode.second.materialParams.mParameterValues, aMaterialFactory);
			mode.second.geometry = getGeometry(aGeometryFactory, mode.second.materialParams.mRenderStyle);
		}
	}
protected:
	std::vector<VertexColor> mInstanceAttributes;
};

