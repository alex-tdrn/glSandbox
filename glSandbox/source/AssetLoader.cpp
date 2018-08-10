#include "AssetLoader.h"
#include "Mesh.h"
#include "Resources.h"
#include "Prop.h"

#include <fx/gltf.h>
#include <numeric>
using namespace fx;

uint32_t calculateElementSize(gltf::Accessor const& accessor);
uint32_t componentSize(gltf::Accessor::Type type);
GLenum gltfToGLType(gltf::Accessor::ComponentType type);

void loadGLTF(std::string const& filename)
{
	gltf::ReadQuotas readQuota;
	readQuota.MaxBufferByteLength = std::numeric_limits<uint32_t>::max();
	gltf::Document doc = gltf::LoadFromText(filename, readQuota);
	
	for(auto const& mesh : doc.meshes)
	{
		for(auto const& primitive : mesh.primitives)
		{
			Mesh::Attributes attributes;
			for(auto const& attribute : primitive.attributes)
			{
				Mesh::Attributes::AttributeBuffer attributeBuffer;
				if(attribute.first == "POSITION")
					attributeBuffer.attributeType = Mesh::AttributeType::positions;
				else if(attribute.first == "NORMAL")
					attributeBuffer.attributeType = Mesh::AttributeType::normals;
				else if(attribute.first == "TEXCOORD_0")
					attributeBuffer.attributeType = Mesh::AttributeType::texcoords;
				else
					continue;
				auto const& accessor = doc.accessors[attribute.second];
				auto const& bufferView = doc.bufferViews[accessor.bufferView];
				auto const& buffer = doc.buffers[bufferView.buffer];
				auto const elementSize = calculateElementSize(accessor);
				attributeBuffer.data = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
				attributeBuffer.size = accessor.count * elementSize;
				if(bufferView.byteStride > elementSize)//interleaved data
				{
					attributeBuffer.stride = bufferView.byteStride;
					attributeBuffer.offset = accessor.byteOffset;
					attributes.interleaved = true;
					attributes.data = &buffer.data[bufferView.byteOffset];
					attributes.size = bufferView.byteLength;
				}
				else
					attributeBuffer.stride = elementSize;
				attributeBuffer.componentSize = componentSize(accessor.type);
				attributeBuffer.dataType = gltfToGLType(accessor.componentType);
				attributes.array[attributeBuffer.attributeType] = std::move(attributeBuffer);
			}
			std::optional<Mesh::IndexBuffer> indices = std::nullopt;
			if(primitive.indices != -1)
			{
				indices = Mesh::IndexBuffer{};
				auto const& accessor = doc.accessors[primitive.indices];
				auto const& bufferView = doc.bufferViews[accessor.bufferView];
				auto const& buffer = doc.buffers[bufferView.buffer];
				indices->data = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
				indices->count = accessor.count;
				indices->size = accessor.count * calculateElementSize(accessor);
				indices->dataType = gltfToGLType(accessor.componentType);
			}

			GLenum drawMode = [&](){
				switch(primitive.mode)
				{
					case gltf::Primitive::Mode::Points:
						return GL_POINTS;
					case gltf::Primitive::Mode::Lines:
						return GL_LINES;
					case gltf::Primitive::Mode::LineLoop:
						return GL_LINE_LOOP;
					case gltf::Primitive::Mode::TriangleFan:
						return GL_TRIANGLE_FAN;
					case gltf::Primitive::Mode::TriangleStrip:
						return GL_TRIANGLE_STRIP;
					default:
						return GL_TRIANGLES;
				}
			}();

			resources::meshes.emplace_back(drawMode, std::move(attributes), std::move(indices));
		}
	}
	std::vector<std::unique_ptr<Node>> nodes;
	for(int i = 0; i < resources::meshes.size(); i++)
		nodes.emplace_back(new Prop{i});
	resources::scenes.emplace_back(std::move(nodes));
}
GLenum gltfToGLType(gltf::Accessor::ComponentType type)
{
	switch(type)
	{
		case gltf::Accessor::ComponentType::Byte:
			return GL_BYTE;
		case gltf::Accessor::ComponentType::UnsignedByte:
			return GL_UNSIGNED_BYTE;
		case gltf::Accessor::ComponentType::Short:
			return GL_SHORT;
		case gltf::Accessor::ComponentType::UnsignedShort:
			return GL_UNSIGNED_SHORT;
		case gltf::Accessor::ComponentType::Float:
			return GL_FLOAT;
		case gltf::Accessor::ComponentType::UnsignedInt:
			return GL_UNSIGNED_INT;
	}
	assert(0);
}

uint32_t componentSize(gltf::Accessor::Type type)
{
	switch(type)
	{
		case gltf::Accessor::Type::Scalar:
			return 1;
		case gltf::Accessor::Type::Vec2:
			return 2;
		case gltf::Accessor::Type::Vec3:
			return 3;
		case gltf::Accessor::Type::Vec4:
		case gltf::Accessor::Type::Mat2:
			return 4;
		case gltf::Accessor::Type::Mat3:
			return 9;
		case gltf::Accessor::Type::Mat4:
			return 16;
	}
	assert(0);
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
	return elementSize * componentSize(accessor.type);
}