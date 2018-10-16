#pragma once
#include "Named.h"
#include "Util.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <optional>
#include <array>
#include <vector>

class Mesh
{
public:
	enum AttributeType
	{
		positions,
		normals,
		tangents,
		texcoords,
		N
	};
	struct Attributes
	{
		struct AttributeBuffer
		{
			uint8_t const* data;
			uint64_t size;
			uint32_t stride;
			uint64_t offset;
			uint32_t componentSize;
			GLenum dataType;
			AttributeType attributeType;
		};
		std::array<std::optional<AttributeBuffer>, AttributeType::N> array;
		bool interleaved = false;
		uint8_t const* data;
		uint64_t size;
	};
	struct IndexBuffer
	{
		uint8_t const* data;
		uint64_t size;
		uint32_t count;
		GLenum dataType;
	};

private:
	mutable unsigned int VAO = 0;
	mutable unsigned int VBO = 0;
	mutable unsigned int EBO = 0;
	GLenum drawMode = GL_POINTS;
	uint32_t const vertexCount;
	uint32_t const indexCount;
	GLenum const indexDataType;
	bool indexedDrawing;
	Bounds const bounds;
	bool availableAttributes[AttributeType::N];

public:
	Name<Mesh> name{"mesh"};

public:
	Mesh(Bounds bounds, GLenum drawMode, Attributes&& attributes, std::optional<IndexBuffer>&& indices = std::nullopt);
	Mesh(Mesh const&) = delete;
	Mesh(Mesh&&);
	~Mesh();
	Mesh& operator=(Mesh const&) = delete;
	Mesh& operator=(Mesh&&) = delete;

public:
	bool hasAttribute(AttributeType attributeType) const;
	Bounds const& getBounds() const;
	void use() const;
	void drawUI();

};

struct SimpleVertex
{
	glm::vec3 position;
};

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoords;
};

template <typename T>
Mesh::IndexBuffer buildIndexBuffer(std::vector<T>&& indices)
{
	Mesh::IndexBuffer indexBuffer;
	indexBuffer.data = reinterpret_cast<uint8_t const*>(indices.data());
	indexBuffer.count = indices.size();
	indexBuffer.size = indices.size() * sizeof(T);
	switch(sizeof(T))
	{
		case 1:
			indexBuffer.dataType = GL_UNSIGNED_BYTE;
			break;
		case 2:
			indexBuffer.dataType = GL_UNSIGNED_SHORT;
			break;
		case 4:
			indexBuffer.dataType = GL_UNSIGNED_INT;
			break;
	}
	return indexBuffer;
}

Mesh::Attributes buildAttributes(std::vector<SimpleVertex>&& vertices);

Mesh::Attributes buildAttributes(std::vector<Vertex>&& vertices);

template <typename T>
Bounds calculateBounds(std::vector<T> const& vertices)
{
	Bounds bounds;
	for(auto const& vertex : vertices)
		bounds += vertex.position;
	return bounds;
}