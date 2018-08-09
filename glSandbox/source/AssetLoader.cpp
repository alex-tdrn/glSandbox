#include "AssetLoader.h"
#include "Mesh.h"
#include "Resources.h"

#include <fx/gltf.h>
using namespace fx;

uint32_t calculateElementSize(gltf::Accessor const& accessor);

void loadGLTF(std::string const& filename)
{
	gltf::Document doc = gltf::LoadFromText(filename);
	
	auto const& mesh = doc.meshes[0];
	auto const& primitive = mesh.primitives[0];
	VertexBuffer vertices;
	for(auto const& attribute : primitive.attributes)
	{
		auto const& accessor = doc.accessors[attribute.second];
		auto const& bufferView = doc.bufferViews[accessor.bufferView];
		auto const& buffer = doc.buffers[bufferView.buffer];
		auto const elementSize = calculateElementSize(accessor);
		VertexBuffer::Attribute att;
		att.data = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
		att.size = accessor.count * elementSize;
		att.stride = elementSize;
		if(attribute.first == "POSITION")
			vertices.positions = att;
		else if(attribute.first == "NORMAL")
			vertices.normals = att;
		else if(attribute.first == "TEXCOORD_0")
			vertices.textureCoords = att;
	}
	auto const& accessor = doc.accessors[primitive.indices];
	auto const& bufferView = doc.bufferViews[accessor.bufferView];
	auto const& buffer = doc.buffers[bufferView.buffer];
	IndicesBuffer indices;
	indices.data = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
	indices.size = accessor.count * calculateElementSize(accessor);
	resources::gltfMesh = new Mesh{std::move(vertices), std::move(indices), nullptr};
}

uint32_t calculateElementSize(gltf::Accessor const& accessor)
{
	uint32_t elementSize = 0;
	switch(accessor.componentType)
	{
		case gltf::Accessor::ComponentType::Byte:
		case gltf::Accessor::ComponentType::UnsignedByte:
			elementSize = 1;
			break;
		case gltf::Accessor::ComponentType::Short:
		case gltf::Accessor::ComponentType::UnsignedShort:
			elementSize = 2;
			break;
		case gltf::Accessor::ComponentType::Float:
		case gltf::Accessor::ComponentType::UnsignedInt:
			elementSize = 4;
			break;
	}
	switch(accessor.type)
	{
		case gltf::Accessor::Type::Scalar:
			return elementSize;
		case gltf::Accessor::Type::Vec2:
			return 2 * elementSize;
		case gltf::Accessor::Type::Vec3:
			return 3 * elementSize;
		case gltf::Accessor::Type::Vec4:
		case gltf::Accessor::Type::Mat2:
			return 4 * elementSize;
		case gltf::Accessor::Type::Mat3:
			return 9 * elementSize;
		case gltf::Accessor::Type::Mat4:
			return 16 * elementSize;
	}
	return 0;
}