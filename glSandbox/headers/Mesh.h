#pragma once
#include "Shader.h"
#include "Asset.h"

#include <optional>
#include <array>

class Mesh : public Asset<Mesh>
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
	mutable unsigned int VAO;
	mutable unsigned int VBO;
	mutable unsigned int EBO;
	GLenum drawMode = GL_POINTS;
	uint32_t const vertexCount;
	uint32_t const indexCount;
	GLenum const indexDataType;
	bool indexedDrawing;

public:
	Mesh(GLenum drawMode, Attributes&& attributes, std::optional<IndexBuffer>&& indices = std::nullopt);
	Mesh(Mesh const&) = delete;
	Mesh(Mesh&&);
	Mesh& operator=(Mesh const&) = delete;
	Mesh& operator=(Mesh&&) = delete;
	~Mesh();

public:
	void use() const override;
	bool drawUI() override;

};