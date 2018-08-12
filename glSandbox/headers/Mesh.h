#pragma once
#include "Named.h"
#include "Util.h"

#include <glad/glad.h>
#include <optional>
#include <array>

class Mesh
{
public:
	enum AttributeType
	{
		positions,
		normals,
		texcoords,
		N
	};
	struct Attributes
	{
		struct AttributeBuffer
		{
			uint8_t const* data;
			uint32_t size;
			uint32_t stride;
			uint32_t offset;
			GLenum dataType;
			uint32_t componentSize;
			AttributeType attributeType;
		};
		std::array<std::optional<AttributeBuffer>, AttributeType::N> array;
		bool interleaved = false;
		uint8_t const* data;
		uint32_t size;
	};
	struct IndexBuffer
	{
		uint8_t const* data;
		uint32_t size;
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
	Bounds const& getBounds() const;
	void use() const;
	void drawUI();

};