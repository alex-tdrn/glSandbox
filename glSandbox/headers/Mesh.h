#pragma once
#include "Shader.h"
#include "NamedAsset.h"

#include <glm/glm.hpp>
#include <vector>

class Material;

struct VertexBuffer
{
	struct Attribute
	{
		uint8_t const* data;
		uint32_t stride;
		uint32_t size;
	};
	Attribute positions;
	Attribute normals;
	Attribute textureCoords;
};

struct IndicesBuffer
{
	uint8_t const* data;
	uint32_t size;
};

class Mesh : public NamedAsset<Mesh>
{
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	VertexBuffer const vertices;
	IndicesBuffer const indices;
	Material* material;
public:
	Mesh(VertexBuffer&& vertices, IndicesBuffer&& indices, Material* material);
	~Mesh();

public:
	void draw(Shader shader) const;
	[[nodiscard]] bool drawUI();
};